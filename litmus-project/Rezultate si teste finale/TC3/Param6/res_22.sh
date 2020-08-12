#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 2 -t table-driven -m 27 '[0,6)' '[9,15)' '[18,24)'
st-trace-schedule my-trace

