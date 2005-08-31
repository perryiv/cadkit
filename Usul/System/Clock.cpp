
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that abstracts the system time.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/Clock.h"
#include "Usul/Types/Types.h"


#ifdef __APPLE__
#include "time.h"
#endif

#include <ctime>

using namespace Usul;
using namespace Usul::System;


Usul::Types::Uint64 Usul::System::milliseconds() 
{
#ifdef _WIN32
  return ::clock();
#elif defined(__APPLE__)
  struct timeval t1;
  gettimeofday(&t1, NULL);
  Usul::Types::Uint64 seconds ( t1.tv_sec );
  Usul::Types::Uint64 microSec ( t1.tv_usec );
  seconds *= 1000;
  microSec /= 1000;
  return seconds + microSec; // Both in milliseconds at this point.
#else
  TODO
#endif
}