#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 3 -c 2 -t table-driven -m 24 '[0,2)' '[8,10)' '[16,18)'
resctl -n 0 -c 2 -t table-driven -m 24 '[2,8)'
st-trace-schedule my-trace

