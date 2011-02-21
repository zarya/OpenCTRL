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
        self.len = 0
        self.checksum = 0
        self.data = []

    def check_checksum(self):
        
        checksum_data = checksum(''.join([self.src[0],self.src[1],self.dst[0],self.dst[1],self.len,self.data]))
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
    packet = packet()
    buffert = 0
    packet.src = [ord(s.recv(1)),ord(s.recv(1))]
    packet.dst = [ord(s.recv(1)),ord(s.recv(1))]
    packet.len = ord(s.recv(1))
    data = []
    while (buffert != packet.len):
        buffert=buffert+1
        data.append(s.recv(1))
    packet.data = data
    packet.checksum = "%s%s" %(s.recv(1),s.recv(1))
    print "Packet data: %s" % ''.join(packet.data)
    if packet.check_checksum: 
        print "Packet Checksum: valid"
    else:
        print "Packet Checksum: invalid"
    bla = 0
    print "Packet len: %s" % packet.len
    print "Packet src_net: %s" % packet.src[0]
    print "Packet src_host: %s" % packet.src[1]
    print "Packet dst_net: %s" % packet.dst[0]
    print "Packet dst_host: %s" % packet.dst[1]
s.close()
