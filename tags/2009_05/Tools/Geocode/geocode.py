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

def getPrecision(dom):
    result = dom.getElementsByTagNameNS('urn:yahoo:maps', 'Result')
    if not result:
        return "Error"
    return result[0].getAttribute('precision')

def getLatitude(dom):
    result = dom.getElementsByTagName('Latitude')
    if not result:
        return "0.0"
    return result[0].firstChild.data

def getLongitude(dom):
    result = dom.getElementsByTagName('Longitude')
    if not result:
        return "0.0"
    return result[0].firstChild.data

class Geocode:
    url = 'http://api.local.yahoo.com/MapsService/V1/geocode'
    appid = 'voundoHV34Hf29AsVm7eelpkHH4NFoxyBSZmk.9dZ5lmX3aWwfiUkrkOgoxRYRTE'
    def __init__(self):
        self.precision = ""
        self.latitude = 0.0
        self.longitude = 0.0

def geocode(street, city, state, zip):
    g = Geocode()

    params = urllib.urlencode({
        'appid': Geocode.appid,
        'street': street,
        'city': city,
        'state' : state,
        'zip' : zip
        })

    data = urllib.urlopen(Geocode.url, params)

    dom = minidom.parse(data)
    
    g.precision =  getPrecision(dom)
    g.latitude  =  getLatitude(dom)
    g.longitude =  getLongitude(dom)
    
    return g

def main():
    try:
        options, args = getopt.getopt(sys.argv[1:],  '', ['street=', 'city=', 'state=', 'zip='])
    except getopt.error:
        print 'Invalid option'
        usage()
        sys.exit(0)

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

    if street=='' and city=='' and state=='' and zip=='':
        print 'Error: Must enter at least one agrument.'
        usage()
        sys.exit(0)

    g = geocode ( street, city, state, zip )

    print 'Precision: ', g.precision
    print 'Latitude:', g.latitude
    print 'Longitude:', g.longitude

if __name__ == "__main__":
    main()
    
