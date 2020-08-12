#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 2 -t table-driven -m 10 '[0,2)' '[5,7)'
st-trace-schedule my-trace

