
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A single error.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Errors/Error.h"
#include "Usul/System/Clock.h"
#include "Usul/Threads/ThreadId.h"

#include <sstream>

using namespace Usul::Errors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Error::Error ( const std::string &m ) : 
  _message ( m ),
  _thread ( Usul::Threads::currentThreadId() ),
  _milliseconds ( Usul::System::Clock::milliseconds() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Error::Error ( const Error &e ) : 
  _message ( e._message ),
  _thread ( e._thread ),
  _milliseconds ( e._milliseconds )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Error::~Error()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the error into a single string.
//
///////////////////////////////////////////////////////////////////////////////

void Error::format ( std::string &s ) const
{
  std::ostringstream out;
  out << _message << ", Thread = " << _thread << ", Milliseconds = " << _milliseconds << '\n';
  s = out.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the error into a single string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Error::format() const
{
  std::string s;
  this->format ( s );
  return s;
}
