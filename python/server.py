import serial
import socket
import time
import struct
from threading import Thread
from libs.openctrl import Packet, checksum
from libs.openctrl_rs232 import Bus

def receiving(ser):
    global sending 
    packet = Packet() #Start packet class
    while True:
        if ser.inWaiting() == 0 && sending == False:
            continue
        #Recieve data from application master
        buffert = 0
        try:
            packet.src = [ord(ser.read(1)),ord(ser.read(1))]    #recieve source address
            packet.dst = [ord(ser.read(1)),ord(ser.read(1))]    #recieve destination address
            packet.id = ord(ser.read(1))                        #recieve packet id
            packet.len = ord(ser.read(1))                       #recieve packet len
        except:
            #if some of above fails start all over
            packet = object
            packet = Packet()
            ser.flushInput()
            continue

        #Start recieving packet data
        if packet.len > 32:
            packet.type = packet.len
            packet.len = 0
        else:
            while (buffert != packet.len):
                buffert=buffert+1
                packet.data.append(ser.read(1)) # add the recieved data to the packet

        packet.checksum = []
        packet.checksum.append(ord(ser.read(1)))
        packet.checksum.append(ord(ser.read(1))) #read checksum from the bus

        print "Packet data: %s" % ''.join(packet.data)
        #Check if the checksum is ok
        if packet.check_checksum() == 1: 
            print "Packet Checksum: valid"
        else:
            print "Packet Checksum: invalid"
            #Stop processing packet en begin maar opnieuw
            packet = object
            packet = Packet()
            continue

        print "Checksum: input: %s %s Output: %s %s" % (packet.checksum[0],packet.checksum[1],packet.checksum_data[0],packet.checksum_data[1])

        #Packet data
        print "Packet len: %s" % packet.len
        print "Packet Type: %s" % packet.type
        print "Packet ID: %s" % (packet.id)
        print "Packet src_net: %s" % packet.src[0]
        print "Packet src_host: %s" % packet.src[1]
        print "Packet dst_net: %s" % packet.dst[0]
        print "Packet dst_host: %s" % packet.dst[1]
        print "Buffer size: %s" % ser.inWaiting()

        time.sleep(0.5)

        bus = Bus(ser) #Create a bus for returning data
        if packet.type == 253:
            ser.flushInput()
            time.sleep(2)
            bus.send_welcome(packet)
            time.sleep(2)
            bus.send_ping(5)
    
    #wait for return data
    
    #return data to application master 
if __name__ ==  '__main__':
    sending = False
    ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
    Thread(target=receiving, args=(ser,)).start()
