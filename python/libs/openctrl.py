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
