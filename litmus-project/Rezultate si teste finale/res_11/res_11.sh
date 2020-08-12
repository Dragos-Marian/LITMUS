#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 2 -c 1 -t table-driven -m 72 '[0,3)' '[18,21)' '[36,39)' '[54,57)'
resctl -n 4 -c 1 -t table-driven -m 72 '[3,9)' '[39,45)'
resctl -n 1 -c 1 -t table-driven -m 72 '[9,17)'
st-trace-schedule my-trace

