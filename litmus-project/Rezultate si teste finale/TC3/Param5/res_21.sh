#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 1 -t table-driven -m 27 '[0,3)' '[9,12)' '[18,21)'
resctl -n 5 -c 1 -t table-driven -m 27 '[3,6)'
st-trace-schedule my-trace

