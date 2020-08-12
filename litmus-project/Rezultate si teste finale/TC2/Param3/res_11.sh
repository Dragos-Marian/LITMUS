#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 5 -c 1 -t table-driven -m 28 '[0,3)' '[14,17)'
resctl -n 3 -c 1 -t table-driven -m 28 '[3,7)'
st-trace-schedule my-trace

