import SocketServer

def sendMSG(src_net,src_host,dst_net,dst_host,data):
    _len = len(data)
    crc = 88
    packet=[1,src_net,src_host,dst_net,dst_host,_len]
    return  "%s%s%s" % (''.join(map(chr, packet)),''.join(map(chr, data)),crc)

class MyTCPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        data = [55,55,55,55]
        self.packet = sendMSG(01,01,02,01,data)
        self.request.send(self.packet)
        return

if __name__ == "__main__":
    HOST, PORT = "localhost", 3001
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
    server.serve_forever()
