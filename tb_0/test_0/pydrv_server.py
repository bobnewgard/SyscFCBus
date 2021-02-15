#!/usr/bin/python2.7
#
# Copyright 2013-2021 Robert Newgard
#
# This file is part of SyscFCBus.
#
# SyscFCBus is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SyscFCBus is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SyscFCBus.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Subclasses Pydrv to provide server executable and test callbacks
"""
#
from struct       import *
from pydrv        import *
from scapy.all    import *
#
# ------------------------------------------------------------------------------
class Payload (Padding):
    name = "Payload"
#
# ------------------------------------------------------------------------------
def get_byte_list(scapy_pkt):
    """
    Converts scapy packet to a list of strings, with each string
    encoding a byte value in hex
    """
    scapy_pkt_str = str(scapy_pkt)
    byte_list     = []
    #
    for char in scapy_pkt_str:
        hex_str   = "%02X" % (ord(char))
        byte_list.append(hex_str)
    #
    return byte_list
#
# ------------------------------------------------------------------------------
class dot3_incr_len(PydrvCallback):
    """
    Packet data callback
        parm: {}
        data: {frame_len: %d, frame: ["XX", ...]}
    """
    __size = 0
    #
    def __init__(self):
        PydrvCallback.__init__(self)
        self.__size = 50
    #
    def cb(self, req_str):
        raw = ""
        #
        if False:
            print >> sys.stderr, "[INF] dot3_incr_len() callback"
        #
        for i in range (self.__size):
            j = (i % 256)
            raw = raw + chr(j)
        #
        L2     = Dot3(dst="CA:BB:BB:BB:BB:BB", src="5A:AA:AA:AA:AA:AA")/Payload(load=raw)
        L2.len = self.__size
        blst   = get_byte_list(L2)
        bcnt   = len(blst)
        #
        ret = {"frame_len": bcnt, "frame": blst}
        #
        if (int(self.__size) >= 1500):
            self.__size = 64
        else:
            self.__size = self.__size + 1
        #
        return json.dumps(ret)
    #
#
# ------------------------------------------------------------------------------
if (__name__ == "__main__"):
    disp      = PyDrv()
    cb_test_1 = dot3_incr_len()
    #
    disp.register(cb_test_1)
    disp.serve()
    #
    exit(0)
#
