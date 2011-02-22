import SocketServer

def getchecksum(data):
    checksum = 0
    i = 0
    while i < len(data):
        try:
            checksum = checksum + ord(data[i])
        except:
            checksum = checksum + data[i]
        i += 1
    return [((checksum>>8)&255),((checksum)& 255)]

def sendMSG(src_net,src_host,dst_net,dst_host,packetid,data):
    _len = len(data)
    checksum = getchecksum("%s%s"%(''.join(map(str,[src_net,src_host,dst_net,dst_host,packetid])),data)) 
    packet=[src_net,src_host,dst_net,dst_host,packetid,_len]
    print id
    return  "%s%s%s" % (''.join(map(chr, packet)),data,''.join(map(chr,checksum)))

class MyTCPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        data = "je moeder op een houtvlot samen met je zusje" 
        self.packet = sendMSG(01,01,02,01,255,data)
        self.request.send(self.packet)
        return

if __name__ == "__main__":
    HOST, PORT = "localhost", 3001
    server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)
    server.serve_forever()
