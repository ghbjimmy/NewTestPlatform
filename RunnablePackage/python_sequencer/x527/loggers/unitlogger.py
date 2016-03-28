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

if __name__=='__main__':
    import sys
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--directory', help='the log directory, default to "/tmp"', default='/tmp')

    args = parser.parse_args()

    ctx = zmq.Context()
    publisher = ZmqPublisher(ctx, "tcp://*:" + str(zmqports.LOGGER_PUB), 'logger')
    g = make_loggers(args.loggers, args.directory, publisher)
    time.sleep(0.2)

    t = LoggerThread(publisher)
    t.logger = g
    t.start()
    sys.stdout.write('press return to end\n')
    sys.stdin.readline()
    t.receiving = False