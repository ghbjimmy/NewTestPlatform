#!/usr/bin/python
__version__='1.0.0'

import zmq

from threading import Thread
from x527 import zmqports
import inspect
from x527.loggers import ZmqPublisher
from x527.tinyrpc.protocols.sequencerrpc import SequencerRPCProtocol
from x527.tinyrpc.server import RPCServer
from x527.tinyrpc.dispatch import RPCDispatcher, public
from x527.tinyrpc.exc import *
from x527.tinyrpc.proxy import TEProxy
from x527.tinyrpc.protocols.jsonrpc import *
from x527.tinyrpc.protocols.terpc import TERPCServerError
from x527.tinyrpc.protocols import jsonrpc
from x527.loggers import reporter
import argparse
import time
from x527.loggers import levels
from x527.loggers import events
from x527.loggers.reporter import Reporter
from sequence import TestSequence, TestGroup
from datetime import datetime
import os
import traceback
from pluginserver import PluginServer
from x527.loggers.reporter import ReportListener

ALLOWED_TRAVELER_KEYS = {'attributes'}
SKIP_ITEM=2

class Sequencer(RPCDispatcher):
    def __init__(self, site, publisher):
        super(Sequencer, self).__init__()
        self.site = site
        self.sequence = None
        self.breakpoints = []
        self.variables = {}
        self.publisher = publisher
        self.test_engine = TEProxy(site, self.publisher)
        self.pc = 0
        self.stop_on_fail = False
        self.running = False
        self.run_thread = None
        self.reporter = Reporter(self.publisher)
        self.plugin = PluginServer()

    @public('connect')
    def connect(self):
        '''connect sequencer to test engine'''
        self.publisher.publish('connecting to test engine...', 'initializing')
        response = self.test_engine.send_cmd(jsonrpc.SERVER_READY, None)
        if response and response.result == '--PASS--':
            self.publisher.publish('connected')
            return True
        else:
            self.publisher.publish('failed to connect to test engine')
            return False

    @public('skip')
    def skip_to_next(self):
        '''return value indicates if we have wrapped around'''
        if self.sequence:
            self.inc_pc()
            item = self.sequence[self.pc]
            return [self.pc+1, str(item)]

    def inc_pc(self):
        '''return value indicates if we have wrapped around'''
        if self.sequence:
            self.pc += 1
            if self.pc == len(self.sequence):
                self.pc = 0
                return True
        return False

    @public('load')
    def load_sequence(self, sequence_db):
        self.sequence = TestSequence(sequence_db)
        self.pc = 0
        return sequence_db + ' has been loaded'

    def jump_to_line(self, line):
        if line > len(self.sequence) or line<1:
            raise JSONRPCInvalidParamsError('the requested line ' + str(line) + ' is beyond the end of the sequence')
        self.pc = line-1
        item = self.sequence[self.pc]
        return [self.pc+1, str(item)]

    def jump_to_name(self, name):
        try:
            item = self.sequence[name]
            if isinstance(item, TestGroup):
                item = item[0]
            self.pc = self.sequence.get_index(item.tid)
            return [self.pc+1, str(item)]
        except KeyError as e:
            raise JSONRPCInvalidParamsError(e)

    @public('jump')
    def s_jump(self, line):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('No sequence has been loaded. Load a valid sequence first')
        try:
            line = int(line)
            return self.jump_to_line(line)
        except ValueError as e:
            #if it's not an integer, assume it's a name
            return self.jump_to_name(line)

    @public('list')
    def s_list(self, lines):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('Can not list. No sequence has been loaded. Load a valid sequence first')
        try:
            start = 0
            stop = 0
            if lines.strip().lower()=='all':
                stop = len(self.sequence)
            else:
                lines = int(lines)
                start = max(self.pc - lines/3, 0)
                stop = min(len(self.sequence), start + lines)
            reply = [[self.pc, start, stop]]
            #line is 1 based and pc is 0 based
            reply.extend([[i+1, str(self.sequence[i])] for i in range(start, stop)])
            return reply
        except ValueError as e:
            raise JSONRPCInvalidParamsError('in command list, ' + lines + ' is not an integer')

    #return True if the run timesout, return False if normal exit. If timeout is 0 then no timeout
    @public('wait')
    def wait(self, timeout):#timeout is in seconds
        timeout = int(timeout)
        t1 = datetime.now()
        while self.running:
            if timeout!=0:
                t2 = datetime.now()
                elasped = (t2-t1).total_seconds()
                if elasped>timeout:
                    return True
            time.sleep(0.1)
        return False

    def run_test(self, test):
        pass_fail = True
        if test.filters:
            for filter_name in test.filters.keys():
                if not test.should_run(filter_name, self.sequence.variables[filter_name]):
                    self.reporter.report(events.ITEM_START, to_pdca=False, **test.row )
                    self.reporter.report(events.ITEM_FINISH, tid=test.tid, result=SKIP_ITEM, value='SKIP', to_pdca=False)
                    return None
        for arg_name in test.kwargs.keys():
            test.kwargs[arg_name] = self.sequence.variables[arg_name]
        self.publisher.publish('running test ' + str(test))
        self.reporter.report(events.ITEM_START, to_pdca=test.report_to_pudding, **test.row )
        if test.call_engine:
            response = self.test_engine.send_test(test)
        else:
            response = self.plugin.call(test)
        if hasattr(response, 'error'):
            raise TERPCServerError('Test Engine error: ' + str(response._jsonrpc_error_code) + ':' + response.error)

        #handle special tags
        r_str = str(response.result).upper().strip()
        #!!! these tests need to be reported to pudding

        if r_str == '--PASS--':
            test.report_to_pudding = True
            pass_fail = True
            # response.result = ''
        if r_str == '--FAIL--':
            test.report_to_pudding = True
            pass_fail = False
            # response.result = ''
        if test.returned_val is not None:
            self.sequence.variables[test.returned_val] = response.result
            if test.judge_pass:
                pass_fail = test.judge_pass(response.result)

        if test.p_attribute is not None:
            self.reporter.report(events.ATTRIBUTE_FOUND, name=test.p_attribute, value=response.result)
            self.sequence.variables[test.p_attribute] = response.result
            time.sleep(5)


        self.reporter.report(events.ITEM_FINISH, tid=test.tid, result=pass_fail, value=response.result, to_pdca=test.report_to_pudding)
        return pass_fail

    def finish_sequence(self, result, error_msg=''):
        log_list = ''
        try:
            response = self.test_engine.send_cmd('end_test', None, timeout=5000)
            if hasattr(response, 'error'):
                error_msg = error_msg + '; error calling end_test: ' + response.error
            else:
                log_list = response.result
        except Exception as e:
            error = e.message
            self.publisher.publish(error)
            error_msg = error_msg + '; error calling end_test: ' + error
            if not isinstance(e, RPCError):
                error_msg = error_msg + os.linesep + traceback.format_exc()
        if result < 0:
            self.reporter.report(events.SEQUENCE_END, result=-1, error=error_msg, logs=log_list)
        else:
            self.reporter.report(events.SEQUENCE_END, result=result, logs=log_list)
        self.running = False
        self.pc = 0

    def run_sequence(self, e_traveler):
        interrupted = False
        wait_for_diags = False
        version ='_'.join([self.sequence.version, __version__])
        self.reporter.report(events.SEQUENCE_START, name=self.sequence.name, version=version)
        if e_traveler is not None:
            for attr, attr_val in e_traveler['attributes'].iteritems():
                self.reporter.report(events.ATTRIBUTE_FOUND, name=attr, value=attr_val)
        total_pass = True
        current_test = None
        try:
            response = self.test_engine.send_cmd('start_test', None, timeout=5000)
            if hasattr(response, 'error'):
                raise TERPCServerError('Test Engine error: ' + str(response._jsonrpc_error_code) + ':' + response.error)
            for test in self.sequence:
                current_test = test
                if not self.running:
                    self.finish_sequence(result=-1, error_msg='sequence aborted')
                    interrupted = True
                    break
                if wait_for_diags and self.stop_on_fail:
                    # the device has failed, we continue to run until the next diags function
                    if test.function == 'diags':
                        break
                result = self.run_test(test)
                self.inc_pc()
                if result is None: #that means this test didn't run
                    continue
                if not result:
                    total_pass = False
                    if test.function == 'detect':
                        wait_for_diags = True
                    else:
                        if self.stop_on_fail:
                            break  #stop on fail unless the function is detect
                current_test = None
        except Exception as e:
            error_msg = e.message
            if current_test:
                error_msg = 'error running test ' + current_test.tid + ', error message is "' + e.message + '"'
                self.reporter.report(events.ITEM_FINISH, result=-1, value='', error=error_msg, tid=current_test.tid, to_pdca=True)
            if not isinstance(e, RPCError):
                error_msg = error_msg + os.linesep + traceback.format_exc()
            self.publisher.publish(error_msg)
            self.finish_sequence(result=False, error_msg=error_msg)
            interrupted = True
        if not interrupted:
            self.finish_sequence(result=int(total_pass))
        self.report_listener.receiving = False

    #listen for UOP, the call back function for report listener
    def received(self, report):
        if report.event == events.REPORT_ERROR:  # handle ERROR events only
            if report.data['error_code'] == reporter.FATAL_ERROR:
                if report.data['site'] == self.site:
                    #stop the sequence for fatal error
                    self.running = False

    @public('run')
    def s_run(self, e_traveler=None):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('trying to run without any sequence loaded')
        self.running = True
        if e_traveler is not None:
            if not isinstance(e_traveler, dict):
                raise JSONRPCInvalidParamsError('etraveler must be a dictionary')
            for key in e_traveler.keys():
                if key not in ALLOWED_TRAVELER_KEYS:
                    raise JSONRPCInvalidParamsError('invalid key "' + key + '" in e_traveler')
                if not isinstance(e_traveler['attributes'], dict):
                    raise JSONRPCInvalidParamsError('in e_traveler, attributes is not a dictionary')
        self.report_listener = reporter.ReportListener(zmqports.LOGGER_PUB)
        self.report_listener.listeners.append(self)
        self.report_listener.start()
        self.run_thread = Thread(target=self.run_sequence, args=(e_traveler,))
        self.run_thread.start()
        return True


    @public('show')
    def show(self, var_name):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('Can not show variable value. No sequence has been loaded. Load a valid sequence first')
        if not self.sequence.variables.has_key(var_name):
            raise  JSONRPCInvalidParamsError('variable ' + var_name + ' does not exist')
        else:
            reply = str(self.sequence.variables[var_name])
        return reply

    @public('s_next')
    def s_next(self):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('No sequence has been loaded. Load a valid sequence first')
        return self.pc+1

    @public('step')
    def step(self):
        if self.sequence is None:
            raise JSONRPCInvalidRequestError('No sequence has been loaded. Load a valid sequence first')
        if self.running:
            raise JSONRPCInvalidRequestError('Can not single step while a test is running')
        if self.pc == len(self.sequence):
            self.pc = 0
            return None
        pass_fail = None

        while pass_fail is None:
            current_item = self.sequence[self.pc]
            self.pc += 1
            pass_fail = self.run_test(current_item)
        return [self.pc, str(current_item)]

    def register_public_methods(self):
        for name, f in inspect.getmembers(
                self, lambda f: callable(f) and hasattr(f, '_rpc_public_name')
        ):
            self.add_method(f, f._rpc_public_name)

    @public('status')
    def status(self):
        if self.running:
            return "RUNNING"
        if self.sequence:
            return 'READY'
        else:
            return 'NONLOADED'

    @public('abort')
    def abort(self):
        if self.running:
            self.running=False
            self.run_thread.join()
            self.publisher.publish('sequence aborted', level = levels.CRITICAL)
        return 'sequence is not running' #do nothing if no tests sequence is being run

    def _dispatch(self, request):
        try:
            try:
                method = self.get_method(request.function)
            except MethodNotFoundError as e:
                return request.error_respond(e)

            # we found the method
            try:
                if request.params:
                    result = method(request.params[0])
                else:
                    result = method()
            except RPCError as e:
                # an error occured within the method, return it
                print e.message, os.linesep, traceback.format_exc()
                return request.error_respond(e)
            # respond with result
            return request.respond(result)
        except Exception as e:
            print e.message, os.linesep, traceback.format_exc()
            return request.error_respond(JSONRPCServerError(e.message + os.linesep + traceback.format_exc()))


class SequencerServer(Thread):
    def __init__(self, site, publisher=None, stop_on_fail=False):
        super(SequencerServer, self).__init__()
        self.site = site

        if publisher:
            self.publisher = publisher
        else:
            ctx = zmq.Context()
            self.publisher = ZmqPublisher(ctx, "tcp://*:" + str(zmqports.SEQUENCER_PUB + site), "Sequencer_" + str(site))
        self.sequencer = Sequencer(site, self.publisher)
        self.sequencer.stop_on_fail = stop_on_fail
        self.dispatcher = self.sequencer
        self.sequencer.register_public_methods()
        time.sleep(1)#give the publisher sometime

        self.rpc_server = RPCServer(
            zmqports.SEQUENCER_PORT + site,
            SequencerRPCProtocol(),
            self.dispatcher,
            self.publisher
        )

    def run(self):
        self.publisher.publish('Sequencer Starting...')
        print 'sequencer starting'
        try:
            self.sequencer.connect()
            self.rpc_server.serve_forever()
            self.publisher.publish('Sequencer Stopped...')
        except Exception as e:
            print 'erro staring the sequencer: ' + e.message
        print 'sequencer stoping'


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    # parser.add_argument('-t', '--test', help='run a test', action='store_true')
    parser.add_argument('-s', '--site', help='the site of the sequencer to connect to', type=int, default=0)
    parser.add_argument('-c', '--continue_on_fail', help='makes the sequencer to continue on fail', action='store_true', default=False)
    args = parser.parse_args()

    stop_on_fail = not args.continue_on_fail
    ss = SequencerServer(args.site, stop_on_fail=(not args.continue_on_fail))
    ss.start()
