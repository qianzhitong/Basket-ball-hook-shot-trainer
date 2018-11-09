#!/bin/bash
#
# Enter Device MAC Address and output file name as required
#
# Note 10 second acquisition period
# Ziyue Yang

gatttool -b C0:83:2A:31:4A:48 -t random --char-write-req --handle=0x0012 --value=0100 --listen > sensor_data_stream_1.dat

#sleep 10 

# pkill gatttool
