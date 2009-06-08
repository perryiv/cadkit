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


class Statistics:
    def __init__(self):
        self.address = 0
        self.street = 0
        self.zip = 0
        self.city = 0
        self.state = 0
        self.error = 0
        self.other = 0
        self.total = 0

    def update(type):
        type = type.tolower()
        if type == 'address':
            self.address = self.address + 1
        elif type == 'street':
            self.street = self.street + 1
        elif type == 'zip':
            self.zip = self.zip + 1
        elif type == 'city':
            self.city = self.city + 1
        elif type == 'state':
            self.state = self.state + 1
        elif type == 'error':
            self.error = self.error + 1
        else:
            self.other = self.other + 1
        self.total = self.total + 1

    def print_stats():
        print 'Address: ', ( self.address / self.total ) * 100, ' %'
        print ' Street: ', ( self.street  / self.total ) * 100, ' %'
        print '    Zip: ', ( self.zip     / self.total ) * 100, ' %'
        print '   City: ', ( self.city    / self.total ) * 100, ' %'
        print '  State: ', ( self.state   / self.total ) * 100, ' %'
        print '  Error: ', ( self.error   / self.total ) * 100, ' %'
        print '  Other: ', ( self.other   / self.total ) * 100, ' %'

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

out.write ( "\"Precision\",\"Latitude\",\"Longitude\"\n" )

stats = Statistics();

for row in reader:
    for s in row:
        if s != '':
            out.write ("\"")
            out.write ( s )
            out.write ("\"")
        out.write ( "," )
    g = geocode.geocode ( row[1], row[3], row[4], row[5] )
    #precision = g.precision
#    print precision
    out.write ( g.precision )
    out.write ( "," )
    out.write ( g.latitude )
    out.write ( "," )
    out.write ( g.longitude )
    out.write ( "\n" )
    #stats.update( precision )

#stats.print_stats()


