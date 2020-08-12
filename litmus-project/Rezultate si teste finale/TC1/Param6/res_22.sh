#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 2 -t table-driven -m 40 '[0,7)' '[20,27)'
resctl -n 5 -c 2 -t table-driven -m 40 '[7,11)'
st-trace-schedule my-trace

