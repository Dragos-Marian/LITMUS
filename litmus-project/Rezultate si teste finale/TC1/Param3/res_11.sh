#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 4 -c 1 -t table-driven -m 21 '[0,2)' '[7,9)' '[14,16)'
resctl -n 3 -c 1 -t table-driven -m 21 '[2,5)'
st-trace-schedule my-trace

