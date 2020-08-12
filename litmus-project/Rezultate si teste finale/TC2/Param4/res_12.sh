#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 5 -c 2 -t table-driven -m 28 '[0,5)' '[14,19)'
resctl -n 3 -c 2 -t table-driven -m 28 '[5,11)'
st-trace-schedule my-trace

