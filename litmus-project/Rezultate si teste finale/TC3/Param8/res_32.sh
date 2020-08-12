#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 4 -c 2 -t table-driven -m 32 '[0,4)' '[16,20)'
st-trace-schedule my-trace

