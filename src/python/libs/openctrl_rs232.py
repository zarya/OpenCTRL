import serial
import socket
import time
import struct
from threading import Thread
from openctrl import Packet, checksum

class Bus(object):
    def __init__(self,ser):
        self.packet = Packet() 
        self.ser = ser

    def send_welcome(self,recv_packet):
        self.packet.src = [1,1]
        self.packet.dst = [0,recv_packet.src[1]]
        self.packet.id = recv_packet.id
        self.packet.len = 252
        self.packet.type = 252
        self.packet.make_checksum()
        self.send_packet()

    def send_ping(self,dst):
        self.packet.src = [1,1]
        self.packet.dst = [1,dst]
        self.packet.id = 1 
        self.packet.len = 255
        self.packet.type = 255
        self.packet.make_checksum()
        self.send_packet()

    def send_packet(self):
        self.ser.write("%s%s%s" % (''.join(map(chr,[self.packet.src[0],self.packet.src[1],self.packet.dst[0],self.packet.dst[1],self.packet.id,self.packet.len])),''.join(map(chr,self.packet.data)),''.join(map(chr,self.packet.checksum))))

    def read(self,size):
        return self.ser.read(size)

