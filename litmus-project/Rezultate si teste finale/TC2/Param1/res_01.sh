#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 1 -t table-driven -m 36 '[0,1)' '[9,10)' '[18,19)' '[27,28)'
resctl -n 1 -c 1 -t table-driven -m 36 '[1,2)' '[13,14)' '[25,26)'
resctl -n 2 -c 1 -t table-driven -m 36 '[2,4)' '[20,22)'
resctl -n 4 -c 1 -t table-driven -m 36 '[4,9)'
st-trace-schedule my-trace

