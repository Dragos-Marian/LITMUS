#!/bin/sh

setsched P-RES


resctl -n 1 -c 1 -t table-driven -m 60 '[0,3)' '[10,13)' '[20,23)' '[30,33)' '[40,43)' '[50,53)'
resctl -n 2 -c 1 -t table-driven -m 60 '[3,5)' '[23,25)' '[43,45)' 
resctl -n 3 -c 1 -t table-driven -m 60 '[5,10)' '[35,40)'



rtspin -v -d 6 -p 1 -r 1 3.16 10 1
rtspin -v -d 7 -p 1 -r 2 2.10 20 1
rtspin -v -d 9 -p 1 -r 3 5.26 30 1
