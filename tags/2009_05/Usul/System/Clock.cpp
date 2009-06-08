
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time values in seconds or milliseconds.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Clock.h"

#include <ctime>

#ifndef _MSC_VER
# include <sys/time.h> // For gettimeofday
#endif 


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the milliseconds offset from a platform dependent value
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Usul::System::Clock::milliseconds() 
{
#ifdef _WIN32
  return ::clock();
#else
  struct timeval t1;
  gettimeofday(&t1, NULL);
  Usul::Types::Uint64 seconds ( t1.tv_sec );
  Usul::Types::Uint64 microSec ( t1.tv_usec );
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the seconds offset from a platform dependent value
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Usul::System::Clock::seconds() 
{
#ifdef _WIN32
  return ::clock()/1000;
#else
  struct timeval t1;
  gettimeofday(&t1, NULL);
  return t1.tv_sec;
#endif
}
