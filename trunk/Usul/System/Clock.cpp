
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time values in seconds or milliseconds
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Clock.h"

// TODO do we really need to include time.h on apple and ctime otherwise?
// Should be able to pick one for all platforms.
#ifdef __APPLE__
# include "time.h"
#else
# include <ctime>
#endif

#ifndef _MSC_VER
# include <sys/time.h> // For gettimeofday
#endif 

using namespace Usul;
using namespace Usul::System;


///////////////////////////////////////////////////////////////////////////////
//
//  Returns the milliseconds offset from a platform dependent value
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint64 Usul::System::Clock::milliseconds() 
{
#ifndef __APPLE__
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

