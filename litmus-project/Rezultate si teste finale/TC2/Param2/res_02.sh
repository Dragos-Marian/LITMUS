#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 2 -t table-driven -m 36 '[0,2)' '[9,11)' '[18,20)' '[27,29)'
st-trace-schedule my-trace

