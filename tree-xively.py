#!/usr/bin/env python

"""
listener.py is an interface program, between an arduino and Pachube. It 
receives JSON-encoded data from the Arduino and uploads it to Pachube.

Note that your API key and pachube feed ID must be in config.ini for this to work!
"""

from twisted.protocols.basic import LineReceiver
from twisted.internet import reactor
from twisted.internet.serialport import SerialPort
from twisted.web import server, resource, client
from twisted.python import usage

import logging
import sys
import time 
import datetime
import simplejson as json
from simplejson import JSONDecodeError
import xively


class THOptions(usage.Options):
    optParameters = [
        ['baudrate', 'b', 9600, 'Serial baudrate'],
        ['port', 'p', '/dev/ttyACM0', 'Serial port to use'],
        ]
                                    
class Echo(LineReceiver):

    api_key = None
    feed_num = 0
        
    def read_config(self):
       
        self.api_key = "_no_Key_"
        self.feed_num = 135383882

    def update_pachube(self, data):
        if not self.api_key:
            self.read_config()

        api = xively.XivelyAPIClient(self.api_key)

        feed = api.feeds.get(self.feed_num)

        now = datetime.datetime.now()
        feed.datastreams = [
            xively.Datastream(id="pm25-tree", current_value=data),
        ]
        feed.update()



    def connectionMade(self):
        logging.info('Serial connection made!')

    def lineReceived(self, line):
        logging.debug('Line: "%s"' % line);

        try:
            data = line;
        except JSONDecodeError, jde:
            logging.exception(jde)
            return

        self.update_pachube(data)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, \
                format='%(asctime)s %(levelname)s [%(funcName)s] %(message)s')

    o = THOptions()
    try:
        o.parseOptions()
    except usage.UsageError, errortext:
        logging.error('%s %s' % (sys.argv[0], errortext))
        logging.info('Try %s --help for usage details' % sys.argv[0])
        raise SystemExit, 1

    if o.opts['baudrate']:
        baudrate = int(o.opts['baudrate'])

    port = o.opts['port']

    logging.debug('About to open port %s' % port)
    s = SerialPort(Echo(), port, reactor, baudrate=baudrate)

    reactor.run()
