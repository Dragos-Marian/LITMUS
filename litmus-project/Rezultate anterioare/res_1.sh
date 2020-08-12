#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 1 -t table-driven -m 60 '[0,3)' '[10,13)' '[20,23)' '[30,33)' '[40,43)' '[50,53)'
resctl -n 1 -c 1 -t table-driven -m 60 '[3,5)' '[23,25)' '[43,45)'
resctl -n 2 -c 1 -t table-driven -m 60 '[5,10)' '[35,40)'
st-trace-schedule my-trace 