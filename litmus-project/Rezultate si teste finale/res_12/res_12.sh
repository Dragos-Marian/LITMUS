#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 2 -t table-driven -m 72 '[0,4)' '[18,22)' '[36,40)' '[54,58)'
resctl -n 1 -c 2 -t table-driven -m 72 '[4,13)'
st-trace-schedule my-trace

