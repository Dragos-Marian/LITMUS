#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 3 -c 1 -t table-driven -m 24 '[0,1)' '[8,9)' '[16,17)'
resctl -n 5 -c 1 -t table-driven -m 24 '[1,3)' '[13,15)'
resctl -n 0 -c 1 -t table-driven -m 24 '[3,8)'
st-trace-schedule my-trace

