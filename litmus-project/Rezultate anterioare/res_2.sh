#!/bin/sh 

 setsched Linux 
 setsched P-RES 


resctl -n 0 -c 1 -t table-driven -m 60 '[0,4)' '[20,24)' '[40,44)'
resctl -n 1 -c 1 -t table-driven -m 60 '[4,10)' '[34,40)'
