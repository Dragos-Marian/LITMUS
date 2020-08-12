#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 1 -t table-driven -m 40 '[0,5)' '[20,25)'
resctl -n 5 -c 1 -t table-driven -m 40 '[5,8)'
st-trace-schedule my-trace

