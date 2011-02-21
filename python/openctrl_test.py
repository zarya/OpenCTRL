import serial
import socket

class packet:
    def __init__(self,src=0,dst=0,len=0):
        self.src_net = 0
        self.src_host = 0
        self.dst_net = 0
        self.dst_host = 0
        self.len = 0

    def set_src(self,net,host):
        self.src_net = int(net)
        self.src_host = int(host)

    def set_dst(self,net,host):
        self.dst_net = int(net)
        self.dst_host = int(host)

    def set_len(self,packet_len):
        self.len = packet_len

def crc16(buff, crc = 0, poly = 0xa001):
    l = len(buff)
    i = 0
    while i < l:
        ch = ord(buff[i])
        uc = 0
        while uc < 8:
            if (crc & 1) ^ (ch & 1):
                crc = (crc >> 1) ^ poly
            else:
                crc >>= 1
            ch >>= 1
            uc += 1
        i += 1
    return crc & 0xff


def process_data(data):
    print data
    #data processing

HOST = 'localhost'    # The remote host
PORT = 3001             # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
bla = 1
while bla == 1:
    byte_receive=s.recv(1)
    if (byte_receive==chr(1)):
        packet = packet()
        buffert = 0
        packet.set_src(ord(s.recv(1)),ord(s.recv(1)))
        packet.set_dst(ord(s.recv(1)),ord(s.recv(1)))
        packet.set_len(ord(s.recv(1)))
        data = []
        while (buffert != packet.len):
            buffert=buffert+1
            data.append(s.recv(1))
        print ''.join(data)
        crc1 = s.recv(1)
        crc2 = s.recv(1) 
        if chr(crc16(data)) == crc1 and ord(crc2) == 255:
            print "CRC Valid: %s %s" % (crc16(data),ord(crc1))
        else:
            print "CRC Invalid: %s %s" % (crc16(data),ord(crc1))
        bla = 0
        print "Packet len: %s" % packet.len
        print "Packet src_net: %s" % packet.src_net
        print "Packet src_host: %s" % packet.src_host
        print "Packet dst_net: %s" % packet.dst_net
        print "Packet dst_host: %s" % packet.dst_host
s.close()
