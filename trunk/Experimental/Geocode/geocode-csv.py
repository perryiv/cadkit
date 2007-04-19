#!/usr/bin/python

###############################################################################
##
##  Copyright (c) 2007, Arizona State University
##  All rights reserved.
##  BSD License: http://www.opensource.org/licenses/bsd-license.html
##  Created by: Adam Kubach
##
###############################################################################

import csv
import sys,string
import geocode

print "Enter a filename for input: ",
filename = string.rstrip(sys.stdin.readline())

reader = csv.reader ( open( filename, "rb" ) )

print "Enter output filename: ",
outfile = string.rstrip(sys.stdin.readline())

out = open( outfile, 'w' )

for s in reader.next():
    out.write("\"")
    out.write(s)
    out.write("\",")

out.write ( "\"Precision\",\"Latitude\",\"Longitude\" \n" )
    
for row in reader:
    for s in row:
        if s != '':
            out.write ("\"")
            out.write ( s )
            out.write ("\"")
        out.write ( "," )
    g = geocode.geocode ( row[1], row[3], row[4], row[5] )
    out.write ( g.precision )
    out.write ( "," )
    out.write ( g.latitude )
    out.write ( "," )
    out.write ( g.longitude )
    out.write ( "\n" )


