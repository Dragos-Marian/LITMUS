#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 3 -c 2 -t table-driven -m 21 '[0,6)'
st-trace-schedule my-trace

