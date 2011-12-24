#!/bin/bash

# easy way:  redirect to here with `ps ux | grep robotview` 
# the 2nd column is list of PID numbers, so grab 2nd arg and ignore the rest
top -d 0.01 -p $2
