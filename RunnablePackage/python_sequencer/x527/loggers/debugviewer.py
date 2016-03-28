__author__ = 'wei'

import zmq
from x527 import zmqports, tinyrpc
from threading import Thread
import time
import levels

import argparse
import threading

class LogSubscriber(Thread):
    def __init__(self, port, listen_to_hearbeat, level, url=None):
        super(LogSubscriber, self).__init__()
        ctx = zmq.Context.instance()
        self.subscriber = ctx.socket(zmq.SUB)
        if url is None:
            url = 'tcp://localhost:' + str(port)
        self.subscriber.connect(url)
        self.url = url
        self.subscriber.setsockopt(zmq.SUBSCRIBE, zmqports.PUB_CHANNEL)
        self.receiving = True
        self.listen_to_heartbeat = listen_to_hearbeat
        self.level = level
        self.lock = threading.Lock()

    def write_out(self, msg):
        self.lock.acquire()
        print msg
        self.lock.release()

    def run(self):
        print 'ready to subscribe to ' + str(self.url)
        while self.receiving:
            try:
                msg = self.subscriber.recv_multipart(zmq.NOBLOCK)
                if  len(msg)==5:
                    topic, ts, level, origin, data = msg[:]
                    if data==tinyrpc.FCT_HEARTBEAT:
                        if self.listen_to_heartbeat:
                            self.write_out('[' + ts + ']\t' + origin + ':' + str(data))
                        continue


                    if int(level) <= self.level:
                        self.write_out('[' + ts + ']\t' + origin + ':' + str(data))
                else:
                    self.write_out(str(msg))
            except zmq.ZMQError:
                pass
            time.sleep(0.02)
        self.subscriber.setsockopt(zmq.LINGER, 0)
        self.subscriber.close()

def list_ports():
    for item in dir(zmqports):
        if not item.startswith('__'):
            print item + ' = ' + str(eval('zmqports.' + item))

if __name__=='__main__':
    import sys
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--ports', help='a comma separted list of ports to listen to, default to 6150', default='6150') #default to listen to site on of the dummy engine
    parser.add_argument('-b', '--heartbeat', help='listends for heartbeat', action="store_true", default=False)
    parser.add_argument('-l', '--listports', help='list all the possible zmq publisher ports', action="store_true", default=False)
    parser.add_argument('-v', '--level', help='the level of verbosity. Only messages with higher priority than level are received', type=int, default=levels.DEBUG)
    parser.add_argument('-r', '--report', help='This viewer only reports formatted reporter message', action="store_true", default=False)


    args = parser.parse_args()

    if args.listports:
        list_ports()
        exit(0)
    if args.report:
        args.level = levels.REPORTER

    ports = args.ports.split(',')
    int_ports = [int(p.strip()) for p in ports]
    subs = []
    for port in int_ports:
        print 'subscribing at port ' + str(port) + '; level is ' + str(args.level)
        s = LogSubscriber(port, args.heartbeat, args.level)
        s.start()
        subs.append(s)
    sys.stdout.write('press return to end\n')
    sys.stdin.readline()

    for s in subs:
        s.receiving = False
