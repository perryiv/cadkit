#!/usr/bin/python

import csv
import sys,string

#p = csv.parser()

print "Enter a filename: ",
filename = string.rstrip(sys.stdin.readline())

file = open ( filename, 'r' )
print file

while 1:
    line = file.readline()
    if not line:
	break
    print line

#for line in file:
#    print line

