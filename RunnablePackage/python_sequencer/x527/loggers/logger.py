__author__ = 'wei'

import sys
import zmq
from x527 import zmqports
from threading import Thread
import time
import levels
import events
import argparse
from reporter import ReporterProtocol
from filelogger import CSVLogger, PivotLogger
from publisher import ZmqPublisher
from puddinglogger import PuddingLogger
from x527.tinyrpc import *


class LoggerThread(Thread):

    event_function_map = {
                          events.SEQUENCE_START:'log_sequence_start',
                          events.SEQUENCE_END:'log_sequence_end',
                          events.ITEM_START:'log_item_start',
                          events.ITEM_FINISH:'log_item_finish',
                          events.ATTRIBUTE_FOUND:'log_attribute_found'
                          }

    def __init__(self, publisher, site_count=6, url=None):
        super(LoggerThread, self).__init__()
        ctx = zmq.Context.instance()
        self.poller = zmq.Poller()
        self.subscribers = [ctx.socket(zmq.SUB) for i in range(site_count)]
        self.logger=None
        self.receiving=True
        self.protocol = ReporterProtocol()
        self.publisher=publisher

        if url is None:
            url = 'tcp://localhost:'
        for i in range(6):
            sub = self.subscribers[i]
            sub.setsockopt(zmq.IDENTITY, str(i))
            sub.connect(url + str(zmqports.SEQUENCER_PUB + i))
            sub.setsockopt(zmq.SUBSCRIBE, zmqports.PUB_CHANNEL)
            self.poller.register(sub, zmq.POLLIN)

    def run(self):
        self.publisher.publish('logger started', level=levels.INFO, id_postfix='logger thread')
        self.heartbeat_at = time.time() + HEARTBEAT_INTERVAL
        while self.receiving:
            try:
                socks = dict(self.poller.poll(1000))
                for sub in self.subscribers:
                    if sub in socks and socks[sub]==zmq.POLLIN:
                        msg = sub.recv_multipart(zmq.NOBLOCK)
                        topic, ts, level, origin, data = msg[:]
                        if int(level) > levels.REPORTER:
                            continue
                        try:
                            report = self.protocol.parse_report(data)
                            print msg
                        except ValueError as e:
                            print 'error parsing the data: ' + str(data) + '; error detail is: ' + e.message
                            continue
                        site = int(sub.getsockopt(zmq.IDENTITY))
                        func_name = self.event_function_map[report.event]
                        func = self.logger.__getattribute__(func_name)
                        func(ts, site, report)
            except zmq.ZMQError as e:
                print e.message
            time.sleep(0.01)
            t_now = time.time()
            if t_now >= self.heartbeat_at:
                self.publisher.publish(FCT_HEARTBEAT, level=levels.INFO, id_postfix='logger thread')
                self.heartbeat_at = t_now + HEARTBEAT_INTERVAL
        self.logger.close()
        for sub in self.subscribers:
            sub.setsockopt(zmq.LINGER, 0)
            sub.close()


def make_loggers(logger_str, path, publisher):
    if len(logger_str) > 3:
        print 'too many loggers ' + str(logger_str) + ', at most 3 are supported'
        exit(-1)

    logger = None
    # PuddingLogger has to be the end of logger chain to process the sequence_end after other file loggers finish
    if 'p' in logger_str:
        p = PuddingLogger(publisher=publisher, next_logger=logger)
        logger = p
    if 'c' in logger_str:
        c = CSVLogger(path, publisher=publisher, next_logger=logger)
        logger = c
    if 'v' in logger_str:
        v = PivotLogger(path, publisher=publisher, next_logger=logger)
        logger = v
    for each in logger_str:
        if each not in ['p','v','c']:
            print 'unsupported logger "'+each+'". The only valid loggers are c[CSVLogger], v[PivotLogger] and p[PuddingLogger]'
    # pass pivot logger to puddinglogger to let it have the pivot path
    if p and v:
        p.pivot_logger = v
    return logger


if __name__=='__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('loggers', help='a list of the loggers your want to enable, currently it can only be a combination of p, \
                                        v, and c. p is the pudding logger, v is the pivot logger, c is the csv logger'
                                        )
    parser.add_argument('-d', '--directory', help='the log directory, default to "/tmp"', default='/tmp')

    args = parser.parse_args()

    ctx = zmq.Context()
    publisher = ZmqPublisher(ctx, "tcp://*:" + str(zmqports.LOGGER_PUB), 'logger')
    g = make_loggers(args.loggers, args.directory, publisher)
    time.sleep(0.2)

    t = LoggerThread(publisher)
    t.logger = g
    t.start()
    #sys.stdout.write('press return to end\n')
    #sys.stdin.readline()
    raw_input()
    t.receiving = False

