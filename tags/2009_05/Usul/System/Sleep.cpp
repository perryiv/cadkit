
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
#include "Usul/Math/MinMax.h"

#ifdef _MSC_VER
# define NOMINMAX
# define WIN32_LEAN_AND_MEAN
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

  // Convert to microseconds
  duration *= 1000;
  
  // Cap just shy of a million.
  // http://www.opengroup.org/onlinepubs/007908799/xsh/usleep.html
  duration = Usul::Math::maximum<unsigned long> ( duration, 999999 );

  ::usleep ( duration );

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

