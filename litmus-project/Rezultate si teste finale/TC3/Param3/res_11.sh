#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 1 -c 1 -t table-driven -m 84 '[0,2)' '[7,9)' '[14,16)' '[21,23)' '[28,30)' '[35,37)' '[42,44)' '[49,51)' '[56,58)' '[63,65)' '[70,72)' '[77,79)'
resctl -n 6 -c 1 -t table-driven -m 84 '[2,6)' '[30,34)' '[58,62)'
resctl -n 8 -c 1 -t table-driven -m 84 '[9,12)' '[51,54)'
st-trace-schedule my-trace

