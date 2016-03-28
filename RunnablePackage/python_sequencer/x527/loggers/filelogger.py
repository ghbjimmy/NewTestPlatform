from datetime import datetime
import os
import re
from reportlogger import *
from publisher import NoOpPublisher
from x527.sequencer.sequencer import SKIP_ITEM

class CSVRun(object):

    def __init__(self,index, ts):
        self.sn = None
        self.items = []
        self.result = False
        self.index = index
        self.start_ts = ts

class CSVLogger(ReportLogger):

    def __init__(self, tester_id, log_folder='/tmp', next_logger=None, site_count=6, publisher=NoOpPublisher()):
        super(CSVLogger, self).__init__()
        self.next_logger = next_logger
        ts = datetime.now()
        file_path = os.path.join(log_folder, datetime.strftime(ts, '%m_%d_%H_%M_%S') + '-csv.csv')
        self.log_f = open(file_path, 'w+')
        self.header_written = False
        self.runs = {i:None for i in range(site_count)}
        self.tester_id = tester_id
        self.log_f.write('tester_id, site, index, start_time, end_time, sn, result, fail_message,')
        self.log_f.flush()
        self.publisher = publisher
        self.next_logger = next_logger

    @call_next
    def log_sequence_start(self, ts, site, report):
        self.runs[site] = CSVRun(ReportLogger.next_index(), ts)
        return

    @call_next
    def log_item_start(self, ts, site, report):
        if not report.data['to_pdca']:
            return
        run = self.runs[site]
        item_data = dict(tid=report.data['tid'], low = report.data['low'], high = report.data['high'])
        run.items.append(item_data)
        return

    @call_next
    def log_item_finish(self, ts, site, report):
        if not report.data['to_pdca']:
            return
        run = self.runs[site]
        item_data = run.items[-1]
        item_data['value'] = report.data.get('value', '')
        return

    @call_next
    def log_attribute_found(self, ts, site, report):
        if report.data['name'].lower() == 'mlbsn' or report.data['name'].lower() == 'eepromsn':
            self.runs[site].sn = report.data['value']

    @call_next
    def log_sequence_end(self, ts, site, report):
        run = self.runs[site]
        if not self.header_written:
            skip_fields = ',,,,,,,,'
            #write tid
            for item in run.items:
                self.log_f.write(item['tid'] + ',')
            self.log_f.write(os.linesep)
            self.log_f.write(skip_fields)
            for item in run.items:
                self.log_f.write(item['low'] + ',')
            self.log_f.write(os.linesep)
            #write  high_limit
            self.log_f.write(skip_fields)
            for item in run.items:
                self.log_f.write(item['high'] + ',')
            self.log_f.write(os.linesep)
            self.header_written=True

        msg = report.data.get('error', '')
        msg = re.sub(',', ';', msg) #got rid of comma in the message
        #now write the result
        self.log_f.write('%s, %d, %d, %s, %s, %s, %s, %s,' % (self.tester_id, site, run.index,
                                                         run.start_ts, ts, run.sn, str(report.data['result']), msg))
        for item in run.items:
            self.log_f.write(item['value'] + ',')
        self.log_f.write(os.linesep)
        self.log_f.flush()
        return

    @call_next
    def close(self):
        self.log_f.close()

class PivotLogger(ReportLogger):

    def __init__(self, log_folder='/tmp', next_logger=None, site_count=6, publisher = NoOpPublisher()):
        super(PivotLogger, self).__init__()
        self.log_folder = log_folder
        ts = datetime.now()
        file_path = os.path.join(log_folder, datetime.strftime(ts, '%m_%d_%H_%M_%S') + '-pivot-acc.csv')
        log_f = open(file_path, 'w+')
        log_f.write('index, site, sn, group, tid, unit, low, high, month, day, hour, minute, second, result, value, fail_msg\n')
        log_f.flush()
        self.log_f_acc = log_f
        self.next_logger = next_logger
        self.devices = {i:None for i in range(site_count)}
        self.publisher = publisher
        self.log_path = ['' for i in range(site_count)]
        self.log_f = [None for i in range(site_count)]
        self.uut_sn = ['' for i in range(site_count)]

    @call_next
    def log_sequence_start(self, ts, site, report):
        self.devices[site] = DUT(ReportLogger.next_index())
        ts = datetime.now()
        self.log_path[site] = os.path.join(self.log_folder, 'UUT{}-'.format(site)+datetime.strftime(ts, '%m_%d_%H_%M_%S') + '-pivot.csv')
        log_f = open(self.log_path[site], 'w+')
        log_f.write('site, sn, group, tid, unit, low, high, month, day, hour, minute, second, result, value, fail_msg\n')
        log_f.flush()
        self.log_f[site] = log_f

    @call_next
    def log_item_start(self, ts, site, report):
        data = report.data
        dut = self.devices[site]
        item = dict(group=data['group'], tid=data['tid'], unit=data['unit'], low=data['low'], high=data['high'], result='',msg='')
        self.devices[site].running_item = item
        return

    @call_next
    def log_item_finish(self, ts, site, report):
        dut = self.devices[site]
        item = dut.running_item
        self.log_f[site].write('%d, %s, %s, %s, %s, %s, %s,' %
                        (site, dut.sn, item['group'], item['tid'],
                        item['unit'], item['low'], item['high']))
        dt = datetime.strptime(ts, '%m-%d_%H:%M:%S.%f')
        self.log_f[site].write(datetime.strftime(dt, '%m, %d, %H, %M, %S,'))
        self.log_f_acc.write('%d, %d, %s, %s, %s, %s, %s, %s,' %
                        (dut.index, site, dut.sn, item['group'], item['tid'],
                        item['unit'], item['low'], item['high']))
        self.log_f_acc.write(datetime.strftime(dt, '%m, %d, %H, %M, %S,'))
        res = report.data['result']
        if res == True:
            self.log_f[site].write('Pass' + ', ')
            self.log_f_acc.write('Pass' + ', ')
        elif res == SKIP_ITEM:
            self.log_f[site].write('Skip' + ', ')
            self.log_f_acc.write('Skip' + ', ')
        else:
            self.log_f[site].write('Fail' + ', ')
            self.log_f_acc.write('Fail' + ', ')
        if report.data.has_key('error'):
            self.log_f[site].write(',' + report.data['error'])
            self.log_f_acc.write(',' + report.data['error'])
        else:
            self.log_f[site].write(str(report.data['value']) + ',')
            self.log_f_acc.write(str(report.data['value']) + ',')
        self.log_f[site].write(os.linesep)
        self.log_f[site].flush()
        self.log_f_acc.write(os.linesep)
        self.log_f_acc.flush()
        self.uut_sn[site] = str(dut.sn)
        return

    @call_next
    def log_attribute_found(self, ts, site, report):
        if report.data['name'].lower() == 'mlbsn':
            self.devices[site].sn = report.data['value']
        return

    @call_next
    def log_sequence_end(self, ts, site, report):
        self.log_f[site].close()
        src = self.log_path[site]
        dst = src.replace('UUT{}'.format(site), self.uut_sn[site])
        os.rename(src, dst)
        self.log_path[site] = dst
        print '=======================>rename pivot log:', self.log_path[site]
        return

    @call_next
    def close(self):
        self.log_f_acc.close()

