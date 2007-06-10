
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
//  Class for printing a string.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Trace/Print.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

using namespace Usul::Trace;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the stream to print to.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  std::ostream *_stream ( 0x0 );
  Usul::Threads::Mutex *_mutex ( 0x0 );
  typedef Usul::Threads::Guard<Usul::Threads::Mutex> Guard;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the string.
//
///////////////////////////////////////////////////////////////////////////////

void Print::execute ( const std::string &s )
{
  if ( 0x0 != Detail::_stream )
  {
    if ( false == s.empty() )
    {
      Detail::Guard guard ( *Detail::_mutex );
      *Detail::_stream << s << std::flush;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the string.
//
///////////////////////////////////////////////////////////////////////////////

void Print::execute ( const char *s )
{
  if ( 0x0 != Detail::_stream )
  {
    if ( 0x0 != s )
    {
      Detail::Guard guard ( *Detail::_mutex );
      *Detail::_stream << s << std::flush;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to print the number.
//
///////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_NUMBER_PRINTING_MEMBER(number_type)\
void Print::execute ( number_type num )\
{\
  if ( 0x0 != Detail::_stream )\
  {\
    Detail::Guard guard ( *Detail::_mutex );\
    *Detail::_stream << num << std::flush;\
  }\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the numbers.
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned long );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned short );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned int );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( long );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( short );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( int );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( double );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( float );


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stream to print to. 
//  Initialize the mutex.
//  Not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Print::init ( std::ostream *s )
{
  Detail::_mutex = Usul::Threads::Mutex::create();
  Detail::_stream = s;
}
