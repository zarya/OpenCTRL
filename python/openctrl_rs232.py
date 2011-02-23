import serial
import socket
import time

def checksum(data):
    checksum = 0
    i = 0
    while i < len(data):
        try:
            checksum = checksum + ord(data[i])
        except:
            checksum = checksum + data[i]
        i += 1
    return [((checksum>>8)&255),((checksum)& 255)]

class Packet(object):
    def __init__(self):
        self.src = [0,0]
        self.dst = [0,0]
        self.id = 0
        self.len = 0
        self.type = 0
        self.checksum = 0
        self.data = []

    def check_checksum(self):
        
        checksum_data = checksum(''.join([self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.len,self.data]))
        if checksum_data[0] == self.checksum[0] and checksum_data[1] == self.checksum[1]:
            return 1
        else:
            return 0

    def make_checksum(self):
        self.checksum = checksum("%s%s"%(''.join(map(chr,[self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.len])),''))

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

    def send_packet(self):
        self.ser.write("%s%s%s" % (''.join(map(chr,[self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len])),''.join(map(chr,self.packet.data)),''.join(map(chr,self.packet.checksum))))
        print "Sending back data:"
        print ''.join(map(str, [self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len]))
        print("%s%s%s" % (''.join(map(chr,[self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len])),''.join(map(chr,self.packet.data)),''.join(map(chr,self.packet.checksum))))
        time.sleep(3)
        print "Done Sleeping"
        

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
    packet.checksum = "%s%s" %(ser.read(1),ser.read(1)) #recieve the packet checksum
    print "Packet data: %s" % ''.join(packet.data)
    #Check if the checksum is ok
    if packet.check_checksum: 
        print "Packet Checksum: valid"
    else:
        #Need to fix something that if invallid request retransmit
        print "Packet Checksum: invalid"
        packet = object
        packet = Packet()
        continue
    #Packet data
    print "Packet len: %s" % packet.len
    print "Packet Type: %s" % packet.type
    print "Packet ID: %s" % (packet.id)
    print "Packet src_net: %s" % packet.src[0]
    print "Packet src_host: %s" % packet.src[1]
    print "Packet dst_net: %s" % packet.dst[0]
    print "Packet dst_host: %s" % packet.dst[1]
    #Push data to RS485 bus or something
    bus = Bus(ser) #Create a bus for returning data
    if packet.type == 253:
        ser.flushInput()
        bus.send_welcome(packet)
        ser.flush()
    
    #wait for return data
    
    #return data to application master 

s.close()
