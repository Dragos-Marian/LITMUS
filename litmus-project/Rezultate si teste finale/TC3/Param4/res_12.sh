#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 8 -c 2 -t table-driven -m 84 '[0,7)' '[42,49)'
st-trace-schedule my-trace

