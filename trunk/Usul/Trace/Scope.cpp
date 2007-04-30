
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
//  Class for Scopeing when entering and leaving a scope.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Trace/Scope.h"
#include "Usul/Trace/Print.h"
#include "Usul/System/Clock.h"
#include "Usul/Threads/ThreadId.h"

#include <iostream>
#include <sstream>

using namespace Usul::Trace;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Scope::Scope ( const void *object, const std::string &n ) : 
  _name   ( n ), 
  _object ( object ),
  _thread ( Usul::Threads::currentThreadId() )
{
  this->_begin();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Scope::Scope ( const std::string &n ) : 
  _name   ( n ), 
  _object ( 0x0 ),
  _thread ( Usul::Threads::currentThreadId() )
{
  this->_begin();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Scope::~Scope()
{
  this->_end();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the beginning of a scope.
//
///////////////////////////////////////////////////////////////////////////////

void Scope::_begin() const
{
  std::ostringstream out;
  out << "Begin " << _thread << ", " << Usul::System::Clock::milliseconds() << ", " << _object << ", " << _name << '\n';
  Usul::Trace::Print::execute ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the end of a scope.
//
///////////////////////////////////////////////////////////////////////////////

void Scope::_end() const
{
  std::ostringstream out;
  out << "  End " << _thread << ", " << Usul::System::Clock::milliseconds() << ", " << _object << ", " << _name << '\n';
  Usul::Trace::Print::execute ( out.str() );
}
