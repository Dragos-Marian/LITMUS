#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 4 -c 1 -t table-driven -m 32 '[0,3)' '[16,19)'
resctl -n 9 -c 1 -t table-driven -m 32 '[3,6)'
st-trace-schedule my-trace

