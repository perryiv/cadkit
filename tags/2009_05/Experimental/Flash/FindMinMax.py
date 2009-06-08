#!/usr/bin/env python

###############################################################################
##
##  Copyright (c) 2008, Arizona State University
##  All rights reserved.
##  BSD License: http://www.opensource.org/licenses/bsd-license.html
##  Created by: Adam Kubach
##
###############################################################################

import FlashDocument
import sys;


###############################################################################
##
##  Print the usage for the script.
##
###############################################################################

def usage():
    print '''FindMinMax.py filename -- Calculate min and max from a FLASH file.'''


###############################################################################
##
##  Main function.
##
###############################################################################

def main( argv ):
	if len(argv) < 2:
		print usage();
		sys.exit(-1);
		
	# Make a document.
	doc = FlashDocument.createDocument();
	doc.read ( argv[1], None, None );
	
	# Make sure there is at least one timestep.
	if 0 == doc.getNumberOfTimeSteps():
		print 'No timesteps in the file.'
		sys.exit ( -1 );
	
	# Load a timestep.
	timestep = doc.loadTimestep ( 0, False );
	
	min = timestep.minimum();
	max = timestep.maximum();
	
	start = 1;
	stop = doc.getNumberOfTimeSteps()
	
	for i in range(1, stop):
		print 'Checking timestep', i
		
		timestep = doc.loadTimestep ( i, False );
		
		mn = timestep.minimum();
		mx = timestep.maximum();
		
		print 'Current min is', mn, '.  Current max is', max
		
		if mn < min:
			min = mn;
		if mx > max:
			max = mx;
			
	
	print 'Minimum: ', min, ' Maximum: ', max


###############################################################################
##
##  Call the main function.
##
###############################################################################

if __name__ == "__main__":
    main(sys.argv)
