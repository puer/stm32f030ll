#!/usr/bin/env python

import serial
import struct

SIGNATURE = "__LL_FAN_".encode("ASCII")

with serial.Serial("COM4", 115200, timeout=0.1) as ser:
    while 1:
        s = ser.read(128)
        if len(s) < 17:
            continue
        (sig, ntc_temp, ntc_reading, set_temp, set_reading) = struct.unpack("<9sHHHH", s)

        if sig == SIGNATURE:
            print(f"NTC - [{ntc_temp}C/{ntc_reading}] SET - [{set_temp}C/{set_reading}]")
