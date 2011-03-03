import cli
from cli.section import command, Root
from openctrl_rs232 import Bus
from openctrl import Packet, checksum

class root(Root):
    @command
    def ping(self,sink,dst):
        """Send ping to slave ping <host>"""
        bus = Bus(self.interface.ser)
        self.sendline(sink,"Sending ping to %s" % dst)
        bus.send_ping(int(dst))

class Interface(cli.Interface):
    root_class=root

    def __init__(self,ser,console):
        super(Interface,self).__init__(name='OpenCTRL(Master)',socket=console)
        self.ser = ser

