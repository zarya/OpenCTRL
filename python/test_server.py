import SocketServer

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


def sendMSG(src_net,src_host,dst_net,dst_host,data):
    _len = len(data)
    crc = [crc16(data),255] 
    packet=[1,src_net,src_host,dst_net,dst_host,_len]
    return  "%s%s%s" % (''.join(map(chr, packet)),data,''.join(map(chr,crc)))

class MyTCPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        data = "je moeder op een houtvlot" 
        self.packet = sendMSG(01,01,02,01,data)
        self.request.send(self.packet)
        return

if __name__ == "__main__":
    HOST, PORT = "localhost", 3001
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
    server.serve_forever()
