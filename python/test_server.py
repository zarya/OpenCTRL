import SocketServer

def getcrc(data):
    crc = 0
    i = 0
    while i < len(data):
        crc = crc + ord(data[i])
        i += 1
    return [((crc>>8)&255),((crc)& 255)]

def sendMSG(src_net,src_host,dst_net,dst_host,data):
    _len = len(data)
    crc = getcrc(data) 
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
