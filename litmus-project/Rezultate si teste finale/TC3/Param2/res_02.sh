#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 7 -c 2 -t table-driven -m 36 '[0,5)'
st-trace-schedule my-trace

