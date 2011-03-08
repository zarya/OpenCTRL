import sys
try:
    import serial
except ImportError:
    print >>sys.stderr, 'pyserial is required, http://pyserial.sourceforge.net/'
    sys.exit(1)
import socket
import time
import struct
import sys

import site
site.addsitedir('libs')

from threading import Thread
from openctrl import Packet, checksum
from openctrl_rs232 import Bus
from interface import Interface 

class Receiver(Thread):
    daemon = True # Start background thread
    
    def __init__(self, ser, options):
        super(Receiver, self).__init__()
        self.ser = ser
        self.options = options
    
    def run(self):
        self.running = True
        packet = Packet() #Start packet class
        bus = Bus(self.ser) #Start bus class
        while self.running:
            if self.ser.inWaiting() == 0:
                time.sleep(0.1) 
                continue
            #Receive data from application master
            buffert = 0
            try:
                packet.src = [ord(bus.read(1)),ord(bus.read(1))]    #recieve source address
                packet.dst = [ord(bus.read(1)),ord(bus.read(1))]    #recieve destination address
                packet.id = ord(bus.read(1))                        #recieve packet id
                packet.len = ord(bus.read(1))                       #recieve packet len
            except:
                #if some of above fails start all over
                packet = object
                packet = Packet()
                self.ser.flushInput()
                continue

            #Start recieving packet data
            if packet.len > 32:
                packet.type = packet.len
                packet.len = 0
            else:
                while (buffert != packet.len):
                    buffert=buffert+1
                    packet.data.append(bus.read(1)) # add the recieved data to the packet

            packet.checksum = []
            packet.checksum.append(ord(bus.read(1)))
            packet.checksum.append(ord(bus.read(1))) #read checksum from the bus

            #Check if the checksum is ok
            if packet.check_checksum() != 1: 
                print "Packet Checksum: invalid"
                #Stop processing packet and restart 
                packet = object
                packet = Packet()
                continue

            if self.options.debug:
                print "Checksum: input: %s %s Output: %s %s" % (packet.checksum[0],packet.checksum[1],packet.checksum_data[0],packet.checksum_data[1])
                print "Packet data: %s" % ''.join(packet.data)
                print "Packet len: %s" % packet.len
                print "Packet Type: %s" % packet.type
                print "Packet ID: %s" % (packet.id)
                print "Packet src_net: %s" % packet.src[0]
                print "Packet src_host: %s" % packet.src[1]
                print "Packet dst_net: %s" % packet.dst[0]
                print "Packet dst_host: %s" % packet.dst[1]
                print "Buffer size: %s" % self.ser.inWaiting()

            time.sleep(0.5)

            bus = Bus(self.ser) #Create a bus for returning data
            if packet.type == 253:
                self.ser.flushInput()
                time.sleep(2)
                bus.send_welcome(packet)
                time.sleep(2)
                bus.send_ping(5)
            if packet.type == 254:
                print "\r\nReceived PONG from: %s" % packet.src[1]

            #Feed application with the received data

def run():
    import optparse
    from cli.console import Console

    parser = optparse.OptionParser()
    parser.add_option('-d', '--debug', action='store_true', default=False,
        dest='debug',
        help='show debugging output (default: no)')
    parser.add_option('-l', '--line', default='/dev/ttyUSB1',
        dest='line',
        help='serial line device (default: /dev/ttyUSB1)')
    parser.add_option('-b', '--baud', default=9600, type='int',
        dest='baudrate',
        help='serial baud rate (default: 9600 bps)')
    parser.add_option('-t', '--timeout', default=1, type='int',
        dest='timeout',
        help='serial read timeout (default: 1 second)')
    
    options, args = parser.parse_args()

    ser = serial.Serial(options.line, options.baudrate, timeout=options.timeout)
    rcv = Receiver(ser,options).start()
    if args and args[0] == "console":
        console = Console() 
        interface = Interface(ser,console) 
        try:
            try:
                interface.loop()
                
            except KeyboardInterrupt:
                rcv.running = False
        finally:
            console.restore()

if __name__ ==  '__main__':
    sys.exit(run())

