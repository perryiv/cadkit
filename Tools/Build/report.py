
#!/usr/bin/python

###############################################################################
##
##  Copyright (c) 2007, Arizona State University
##  All rights reserved.
##  BSD License: http://www.opensource.org/licenses/bsd-license.html
##  Created by: Adam Kubach
##
###############################################################################

import sys, getopt
import os
import time
import popen2

def usage():
    print '''report.py -- build and sent report
    Options:
    --email'''

def main():
    try:
        options, args = getopt.getopt(sys.argv[1:],  '', ['email='])
    except getopt.error:
        print 'Invalid option'
        usage()
        sys.exit(0)

    email = ''

    for a in options[:]:
        if a[0] == '--email':
            email=a[1]

    if email=='':
        print 'Error: Must provide an email address.'
        usage()
        sys.exit(0)

    # Get the directory where the source code lives.
    srcDirectory = os.environ['CADKIT_INC_DIR']
    os.chdir ( srcDirectory );

    # Do an update.
    svn = os.popen ( 'svn update' ).read()

    # TODO: should make the build directory an option.
    os.chdir ( 'CadViewer/build/release' );

    # Do a build.
    r, w, e = popen2.popen3( 'make')
    result = r.read()
    result += e.read()
    r.close()
    e.close()
    w.close()
    #result = os.popen ( 'make' ).read()

    # Include the current time in the subject.
    subject = 'Build results for ' + time.ctime(time.time() )

    # Send the results to given email address.
    os.popen ( 'mail -s "' + subject + '" ' + email, 'w' ).write ( svn + result )

    print svn + result

if __name__ == "__main__":
    main()

