import serial
import socket

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

class packet(object):
    def __init__(self):
        self.src = [0,0]
        self.dst = [0,0]
        self.id = [0,0]
        self.len = 0
        self.checksum = 0
        self.data = []

    def check_checksum(self):
        
        checksum_data = checksum(''.join([self.src[0],self.src[1],self.dst[0],self.dst[1],self.id,self.len,self.data]))
        if checksum_data[0] == self.checksum[0] and checksum_data[1] == self.checksum[1]:
            return 1
        else:
            return 0

HOST = 'localhost'    # The remote host
PORT = 3001             # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
bla = 1
while bla == 1:
    #Recieve data from application master
    packet = packet() #Start packet class
    buffert = 0
    packet.src = [ord(s.recv(1)),ord(s.recv(1))]    #recieve source address
    packet.dst = [ord(s.recv(1)),ord(s.recv(1))]    #recieve destination address
    packet.id = [ord(s.recv(1))]                    #recieve packet id
    packet.len = ord(s.recv(1))                     #recieve packet len
    data = []
    #Start recieving packet data
    while (buffert != packet.len):
        buffert=buffert+1
        data.append(s.recv(1))
    packet.data = data # Add the recieved data to the packet
    packet.checksum = "%s%s" %(s.recv(1),s.recv(1)) #recieve the packet checksum
    print "Packet data: %s" % ''.join(packet.data)
    #Check if the checksum is ok
    if packet.check_checksum: 
        print "Packet Checksum: valid"
    else:
        #Need to fix something that if invallid request retransmit
        print "Packet Checksum: invalid"
    bla = 0
    #Packet data
    print "Packet len: %s" % packet.len
    print "Packet ID: %s" % (packet.id[0]+packet.id[1])
    print "Packet src_net: %s" % packet.src[0]
    print "Packet src_host: %s" % packet.src[1]
    print "Packet dst_net: %s" % packet.dst[0]
    print "Packet dst_host: %s" % packet.dst[1]
    #Push data to RS485 bus or something
    
    #wait for return data
    
    #return data to application master 

s.close()
