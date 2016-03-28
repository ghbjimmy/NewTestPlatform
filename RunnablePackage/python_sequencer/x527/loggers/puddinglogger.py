from pypudding import IPUUT, IPTestSpec, IPTestResult, InstantPuddingError, IPGHStation
import pypudding
from publisher import NoOpPublisher
import shutil
from reporter import Reporter
from reporter import ReporterProtocol
import reporter
import events
import time
import traceback

from reportlogger import *
import zipfile

def check_pudding_exception(aFunc):
    '''decorator to assert the reply represents a successful operation'''
    def asserted_func(*args, **kwargs):
        try:
            aFunc(*args, **kwargs)
        except InstantPuddingError as e:
            sf = args[0]
            site = args[2]
            sf.reporter.report(events.REPORT_ERROR, site=site, error_code=reporter.InstantPuddingError, error_msg=e.message)
            upload_log_file = sf.duts[site].log_file
            upload_log_file.write(e.message + os.linesep + traceback.format_exc())
    asserted_func.__name__=aFunc.__name__
    asserted_func.__doc__=aFunc.__doc__
    return asserted_func

special_results = ['--PASS--', '--FAIL--']
def get_all_files(root_path):
    if os.path.isfile(root_path):
        return [root_path]
    if os.path.isdir(root_path):
        return_list = []
        files = os.listdir(root_path)
        for file in files:
            if file.startswith('.'):
                continue # ignore hidden files
            if os.path.isdir(file):
                return_list.extend(get_all_files(file))
            elif os.path.isfile(file):
                return_list.append(file)
            else:
                continue
        return return_list
    #if the input is not a file not a folder, returns an empty list
    return []

class PuddingUUT(object):
    def __init__(self, site):
        self.site = site
        self.uut = None
        self.current_spec = None
        self.log_file = None
        self.log_file_name = None

class PuddingLogger(object):

    CHECK_UOP_INTERVAL = 5000

    def __init__(self, publisher=NoOpPublisher(), next_logger=None, site_count=6):
        super(PuddingLogger, self).__init__()
        self.duts = [PuddingUUT(i) for i in range(site_count)]
        gh_station = IPGHStation()
        s_type = gh_station[pypudding.IP_STATION_TYPE]
        station_id = gh_station[pypudding.IP_STATION_NUMBER]
        self.station_type = s_type
        self.station_id = station_id
        self.publisher = publisher
        self.next_logger=next_logger
        self.pivot_logger = None
        self.reporter = Reporter(self.publisher)
        self.check_uop_at = time.time() + self.CHECK_UOP_INTERVAL

    def check_UOP(self, site, force=False):
        if not force and self.duts[site].uut.sn:
            if time.time()<self.check_uop_at:
                return
            else:
                self.check_uop_at = time.time() + self.CHECK_UOP_INTERVAL
        try:
            uut = self.duts[site].uut
            if uut is not None:
                uut.amIOkay()
        except InstantPuddingError as e:
            self.reporter.report(events.REPORT_ERROR, site=site, error_code=reporter.FATAL_ERROR, error_msg=e.message)

    def zip_logs(self, log_files, ts, log_folder, sn):
        if len(log_files) == 0:
            return None
        dst = os.path.join(log_folder, sn)
        if os.path.exists(dst):
            shutil.rmtree(dst)
        os.mkdir(os.path.join(log_folder, sn))
        zf_path = os.path.join('/tmp', str(sn)+'_' + ts + '_'+'.zip')
        files = []
        for log_path in log_files:
            files.extend(get_all_files(log_path))
        if len(files)==0:
            return None
        zf = zipfile.ZipFile(zf_path, 'w')
        for file in files:
            f_name = os.path.split(file)[1]
            f_name = sn + '_' + f_name
            dest = os.path.join(dst, f_name)
            shutil.copy(str(file), str(dest))
            zf.write(dest, arcname=os.path.join(sn,f_name), compress_type=zipfile.ZIP_DEFLATED)
        zf.close()
        return zf_path


    @call_next
    @check_pudding_exception
    def log_sequence_start(self, ts, site, report):
        ts = ts.replace(':', '_') #replace : with _ because file names don't like :
        site_dut = self.duts[site]
        old_uut = site_dut.uut
        if old_uut is not None:
            del old_uut
        uut = IPUUT()
        site_dut.log_file_name = '/tmp/site_' + str(site) + '_' + ts + '_' + '_pudding_upload.log'
        site_dut.log_file = open(site_dut.log_file_name, 'w+')
        uut.start()
        uut.set_DUT_position(self.station_id, site+1)
        uut.add_attribute(pypudding.IP_ATTRIBUTE_STATIONSOFTWARENAME, report.data['name'])
        uut.add_attribute(pypudding.IP_ATTRIBUTE_STATIONSOFTWAREVERSION, report.data['version'])
        site_dut.uut = uut
        #there is no need to check amI OKay yet because there is no serial number yet
        return

    @call_next
    @check_pudding_exception
    def log_item_start(self, ts, site, report):
        self.check_UOP(site)
        data = report.data
        self.publisher.publish('in item start: ' + str(data))
        spec = None
        if data['to_pdca']:
            spec = IPTestSpec(str(data['group']),
                              subtest_name=str(data['tid']),
                              limits={'low_limit': data['low'], 'high_limit': data['high']},
                              unit=str(data['unit']))
        else:
            spec = IPTestSpec(str(data['group']),
                              subtest_name=str(data['tid'])) #this kind of test might need to be reported to pudding because it's pass/fail
        old_spec = self.duts[site].current_spec
        if old_spec is not None:
            del old_spec
        self.duts[site].current_spec = spec
        self.publisher.publish('in item start, spec=' + str(spec))
        return

    @call_next
    @check_pudding_exception
    def log_item_finish(self, ts, site, report):
        self.check_UOP(site, force=True)
        data = report.data
        result = None
        if not data['to_pdca']:
            return
        self.publisher.publish('in item fish: ' + str(data))
        if data['result'] < 0:
            error_msg = str(data['error'])
            if len(error_msg)>512:
                error_msg = error_msg[:512]
            result = IPTestResult(pypudding.IP_FAIL, '', error_msg)
        if data['result'] == False:
            if 'error' in data:
                error_msg = str(data['error'])
                if len(error_msg)>512:
                    error_msg = error_msg[:512]
                if data['value'] in special_results:
                    result = IPTestResult(pypudding.IP_FAIL, error_msg)
                else:
                    result = IPTestResult(pypudding.IP_FAIL, data['value'], error_msg)
            else:
                error_msg = str(data['value'])
                if len(error_msg)>512:
                    error_msg = error_msg[:512]
                if data['value'] in special_results:
                    result = IPTestResult(pypudding.IP_FAIL, error_msg)
                else:
                    result = IPTestResult(pypudding.IP_FAIL, data['value'], error_msg)
        if data['result'] == True:
            if data['value'] in special_results:
                result = IPTestResult(pypudding.IP_PASS)
            else:
                result = IPTestResult(pypudding.IP_PASS, data['value'])
        self.publisher.publish('spec='+str(self.duts[site].current_spec))
        self.publisher.publish('result='+str(data['value']))

        self.duts[site].uut.add_result(self.duts[site].current_spec, result)
        old_spec = self.duts[site].current_spec
        del old_spec
        self.duts[site].current_spec = None
        del result
        return

    @call_next
    @check_pudding_exception
    def log_attribute_found(self, ts, site, report):
        self.publisher.publish(str(report))
        attr_name = report.data['name'].upper()
        value = report.data['value']
        if 'MLBSN' in attr_name or 'EEPROMSN' in attr_name:
            self.duts[site].uut.set_sn(value)
            time.sleep(2)
            self.check_UOP(site, force=True)
        elif pypudding.IP_ATTRIBUTE_SERIALNUMBER ==attr_name:
            self.duts[site].uut.set_sn(value)
            time.sleep(2)
            self.check_UOP(site, force=True)
        else:
            self.duts[site].uut.add_attribute(report.data['name'], value)
        return

    def find_log_folder(self, file_list):
        log_files = file_list.split(",")
        log_files = [fn for fn in log_files if len(fn.strip())>0 ]
        if len(log_files) == 0:
            return '/tmp', log_files
        else:
            log_folder = os.path.split(log_files[0])[0]
            return log_folder, log_files

    @call_next
    def log_sequence_end(self, ts, site, report):
        result = report.data['result']
        self.check_UOP(site, force=True)
        site_dut = self.duts[site]
        uut = site_dut.uut
        sn = uut.sn
        upload_log_file = site_dut.log_file
        ts = ts.replace(':', '_') #replace : with _ because file names don't like :
        pivot_log = ''
        if self.pivot_logger:
            pivot_log = self.pivot_logger.log_path[site]
            print '=======================>pivot_log:',pivot_log
        if result >= 0:
            log_folder, log_files = self.find_log_folder(report.data['logs'])
            upload_log_file.write('=======================>log_folder:' + log_folder + '\n')
            upload_log_file.write('=======================>log_files:' + str(log_files) + '\n')
            upload_log_file.flush()
            log_files.append(pivot_log)
            upload_log_file.write('=======================>log_files:' + str(log_files) + '\n')
            upload_log_file.flush()
            try:
                zipped_logs = self.zip_logs(log_files, ts, log_folder, sn)
                upload_log_file.write('=======================>zipped_logs:' + str(zipped_logs) + '\n')
                if zipped_logs:
                    uut.add_blob_file(self.station_type + '_' + sn, zipped_logs)
            except Exception as e:
                upload_log_file.write('======================>error creating zipped file: ' + e.message)
                self.reporter.report(events.REPORT_ERROR, site=site, error_code=reporter.ZIPPED_FILE_ERROR, error_msg=e.message)
            finally:
                upload_log_file.close()
            uut.add_blob_file('pudding_upload_log',  self.duts[site].log_file_name)
            uut.done()
            if result:
                uut.commit(pypudding.IP_PASS)
            else:
                uut.commit(pypudding.IP_FAIL)
        else:
            uut.cancel()
        return

    @call_next
    def close(self):
        return
