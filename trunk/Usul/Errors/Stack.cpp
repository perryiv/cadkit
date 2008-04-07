
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error stack.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Errors/Stack.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include <sstream>

using namespace Usul;
using namespace Usul::Errors;


///////////////////////////////////////////////////////////////////////////////
//
//  Don't want this in the header.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard<Stack::Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Static data member(s).
//
///////////////////////////////////////////////////////////////////////////////

Stack *Stack::_instance ( 0x0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

Stack::Stack() : _s(), _m ( Mutex::create() )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Stack::~Stack()
{
  delete _m;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the one instance.
//
///////////////////////////////////////////////////////////////////////////////

Stack &Stack::instance()
{
  // Guard the creation of the instance. TODO, use double-lock pattern.
  static Mutex *mutex = Mutex::create();
  Guard guard ( *mutex );

  // If no instance yet.
  if ( 0x0 == _instance )
  {
    // Declare new instance.
    _instance = new Stack;
  }

  // Return the instance.
  return *_instance;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the stack.
//
///////////////////////////////////////////////////////////////////////////////

void Stack::clear()
{
  Guard guard ( *_m );
  _s.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is it empty?
//
///////////////////////////////////////////////////////////////////////////////

bool Stack::empty() const
{
  Guard guard ( *_m );
  bool result ( _s.empty() );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

Stack::size_type Stack::size() const
{
  Guard guard ( *_m );
  size_type result ( _s.size() );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push an error.
//
///////////////////////////////////////////////////////////////////////////////

void Stack::push ( const std::string &message )
{
  Guard guard ( *_m );
  _s.push_front ( Usul::Errors::Error ( message ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Push an error.
//
///////////////////////////////////////////////////////////////////////////////

void Stack::push ( const char *message )
{
  Guard guard ( *_m );
  if ( message )
    _s.push_front ( Usul::Errors::Error ( message ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pop the top error.
//
///////////////////////////////////////////////////////////////////////////////

void Stack::pop()
{
  Guard guard ( *_m );
  _s.pop_front();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the top error.
//
///////////////////////////////////////////////////////////////////////////////

Stack::value_type Stack::top() const
{
  Guard guard ( *_m );
  value_type result ( _s.front() );
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the errors into single string.
//
///////////////////////////////////////////////////////////////////////////////

void Stack::format ( const std::string &prefix, std::string &s ) const
{
  Guard guard ( *_m );
  std::ostringstream temp;
  for ( ErrorStack::const_iterator i = _s.begin(); i != _s.end(); ++i )
    temp << prefix << i->format() << '\n';
  s = temp.str();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Format the errors into single string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Stack::format ( const std::string &prefix ) const
{
  std::string s;
  this->format ( prefix, s );
  return s;
}