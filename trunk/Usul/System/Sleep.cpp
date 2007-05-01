
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that abstracts the system sleep function.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Sleep.h"

#ifdef _MSC_VER
# define NOMINMAX
# include <windows.h>
#else
# include <unistd.h>
#endif 


///////////////////////////////////////////////////////////////////////////////
//
//  Sleep for the specified number of milliseconds.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::System::Sleep::milliseconds ( unsigned long duration ) 
{
#ifdef _MSC_VER
  ::Sleep ( duration );
#else
  ::usleep ( duration * 1000 ); // Wants microseconds
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Sleep for the specified number of seconds.
//
///////////////////////////////////////////////////////////////////////////////

void Usul::System::Sleep::seconds ( unsigned long duration ) 
{
  Usul::System::Sleep::milliseconds ( duration * 1000 );
}

