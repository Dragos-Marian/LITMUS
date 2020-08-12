#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 1 -t table-driven -m 36 '[0,1)' '[6,7)' '[12,13)' '[18,19)' '[24,25)' '[30,31)'
resctl -n 3 -c 1 -t table-driven -m 36 '[1,5)' '[13,17)' '[25,29)'
resctl -n 7 -c 1 -t table-driven -m 36 '[7,9)'
st-trace-schedule my-trace

