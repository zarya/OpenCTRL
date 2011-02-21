import serial
import socket

def checksum(data):
    checksum = 0
    i = 0
    while i < len(data):
        checksum = checksum + ord(data[i])
        i += 1
    return [((checksum>>8)&255),((checksum)& 255)]

class packet(object):
    def __init__(self,src=0,dst=0,len=0):
        self.src = [0,0]
        self.dst = [0,0]
        self.len = 0
        self.checksum = 0
        self.data = []

    def check_checksum(self):
        checksum_data = checksum(self.data)
        if checksum_data[0] == self.checksum[0] and checksum_data[1] == self.checksum[1]:
            return 1
        else:
            return 0

    @property
    def len(self):
        return self._len

    @len.setter
    def len(self, value):
        self._len = value

    @len.deleter
    def len(self):
        del self._len

    @property
    def dst(self):
        return self._dst

    @dst.setter
    def dst(self, value):
        self._dst = value 

    @dst.deleter
    def dst(self):
        del self._dst

    @property
    def src(self):
        return self._src

    @src.setter
    def src(self, value):
        self._src = value

    @src.deleter
    def src(self):
        del self._src

    @property
    def checksum(self):
        return self._checksum

    @checksum.setter
    def checksum(self, value):
        self._checksum = value

    @checksum.deleter
    def checksum(self):
        del self._checksum
    
    @property
    def data(self):
        return self._data

    @data.setter
    def data(self, value):
        self._data = value

    @data.deleter
    def data(self):
        del self._data


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
