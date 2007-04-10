#!/usr/bin/python

###############################################################################
##
##  Copyright (c) 2007, Arizona State University
##  All rights reserved.
##  BSD License: http://www.opensource.org/licenses/bsd-license.html
##  Created by: Adam Kubach
##
###############################################################################

import urllib
from xml.dom import minidom
import sys, getopt

def usage():
    print '''geocode.py -- Get lat/long info from address
    Options:
    --street, --city, --state, --zip'''

try:
    options, args = getopt.getopt(sys.argv[1:], 
				  '', ['street=', 'city=', 'state=', 'zip='])
except getopt.error:
    print 'Invalid option'
    usage()
    sys.exit(0)


url = 'http://api.local.yahoo.com/MapsService/V1/geocode'
appid = 'voundoHV34Hf29AsVm7eelpkHH4NFoxyBSZmk.9dZ5lmX3aWwfiUkrkOgoxRYRTE'

street = ''
city = ''
state = ''
zip = ''

for a in options[:]:
    if a[0] == '--street':
	street=a[1]
    elif a[0] == '--city':
	city=a[1]
    elif a[0] == '--state':
	state=a[1]
    elif a[0] == '--zip':
	zip=a[1]
    print a

params = urllib.urlencode({
	'appid': appid,
	'street': street,
	'city': city,
	'state' : state,
	'zip' : zip
})

data = urllib.urlopen(url, params)
dom = minidom.parse(data)

def getPrecision(dom):
    result = dom.getElementsByTagNameNS('urn:yahoo:maps', 'Result')
    return result[0].getAttribute('precision')

def getLatitude(dom):
    result = dom.getElementsByTagName('Latitude')
    return result[0].firstChild.data

def getLongitude(dom):
    result = dom.getElementsByTagName('Longitude')
    return result[0].firstChild.data

print getPrecision(dom)
print getLatitude(dom)
print getLongitude(dom)

