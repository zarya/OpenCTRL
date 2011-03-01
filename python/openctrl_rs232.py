import serial
import socket
import time
import struct

def checksum(data):
    checksum = 0
    i = 0
    while i < len(data):
        try:
            checksum = checksum + ord(data[i])
        except:
            checksum = checksum + data[i]
        i += 1
    return [((checksum)& 255),((checksum>>8)&255)]

class Packet(object):
    def __init__(self):
        self.src = [0,0]
        self.dst = [0,0]
        self.id = 0
        self.len = 0
        self.type = 0
        self.checksum = "00" 
        self.data = []
        self.checksum_data = "00"

    def check_checksum(self):
        if (self.len == 0):
            self.checksum_data = checksum("%s%s"%(''.join(map(chr,[self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.type])),''.join(self.data)))
        else:
            self.checksum_data = checksum("%s%s"%(''.join(map(chr,[self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.len])),''.join(self.data)))
        if self.checksum_data[0] == self.checksum[0] and self.checksum_data[1] == self.checksum[1]:
            return 1
        else:
            return 0

    def make_checksum(self):
        self.checksum = checksum("%s%s"%(''.join(map(chr,[self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.len])),''.join(self.data)))

class Bus(object):
    def __init__(self,ser):
        self.packet = Packet() 
        self.ser = ser

    def send_welcome(self,recv_packet):
        self.packet.src = [1,1]
        self.packet.dst = [0,recv_packet.src[1]]
        self.packet.id = recv_packet.id
        self.packet.len = 252
        self.packet.type = 252
        self.packet.make_checksum()
        self.send_packet()

    def send_ping(self,dst):
        self.packet.src = [1,1]
        self.packet.dst = [0,dst]
        self.packet.id = 1 
        self.packet.len = 255
        self.packet.type = 255
        self.packet.make_checksum()
        self.send_packet()

    def send_packet(self):
        self.ser.write("%s%s%s" % (''.join(map(chr,[self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len])),''.join(map(chr,self.packet.data)),''.join(map(chr,self.packet.checksum))))
        print "Sending back data:"
        print("%s%s%s" % (''.join(map(chr,[self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len])),''.join(map(chr,self.packet.data)),''.join(map(chr,self.packet.checksum))))

bla = 1
ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
packet = Packet() #Start packet class
while bla == 1:
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

    time.sleep(0.1)

    bus = Bus(ser) #Create a bus for returning data
    if packet.type == 253:
        ser.flushInput()
        time.sleep(2)
        bus.send_welcome(packet)
    
    #wait for return data
    
    #return data to application master 

s.close()
