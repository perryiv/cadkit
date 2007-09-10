
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor class for horizontal joystick input.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Functors/Interaction/Input/JoystickHorizontal.h"
#include "Usul/Trace/Trace.h"

using namespace Usul::Functors::Interaction::Input;

USUL_IMPLEMENT_TYPE_ID ( JoystickHorizontal );


///////////////////////////////////////////////////////////////////////////////
//
//  Default constructor.
//
///////////////////////////////////////////////////////////////////////////////

JoystickHorizontal::JoystickHorizontal ( Unknown *caller, const std::string &name ) : 
  BaseClass ( caller, name ),
  _j ( caller )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

JoystickHorizontal::JoystickHorizontal ( const JoystickHorizontal &j ) : BaseClass ( j ),
  _j ( j._j )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

JoystickHorizontal::~JoystickHorizontal()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the Horizontal analog input.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickHorizontal::operator()()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  _value = ( ( _j.valid() ) ? _j->joystickHorizontal() : _value );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the caller.
//
///////////////////////////////////////////////////////////////////////////////

void JoystickHorizontal::caller ( Unknown* caller )
{
  USUL_TRACE_SCOPE;

  // Call the base class.
  BaseClass::caller ( caller );

  // Set our internal query pointer to caller.
  Guard guard ( this->mutex() );
  _j = caller;
}
