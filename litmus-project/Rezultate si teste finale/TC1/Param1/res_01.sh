#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 1 -t table-driven -m 10 '[0,1)' '[5,6)'
resctl -n 1 -c 1 -t table-driven -m 10 '[1,5)'
st-trace-schedule my-trace

