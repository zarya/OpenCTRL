import serial
import socket

def process_data(data):
    #data processing

HOST = '10.38.18.176'    # The remote host
PORT = 3001             # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
while 1:
    byte_receive=s.recv(1)
    if (byte_receive==chr(0)):
        buffert = 0
        source = s.recv(2)
        destination = s.recv(2)
        lenght = ord(s.recv(1))
        while (buffert != lenght):
            buffert=buffert+1+
            data[] = s.recv(1)
            process_data(data)

s.close()
