
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "StopAnimation.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IDldNavigator.h"

USUL_IMPLEMENT_COMMAND ( StopAnimation );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StopAnimation::StopAnimation ( Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller )
{
  USUL_TRACE_SCOPE;
  this->text ( "Stop Animation" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StopAnimation::~StopAnimation ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void StopAnimation::_execute ()
{
  USUL_TRACE_SCOPE;
  //this->_dummy();
#if 1

  Usul::Interfaces::IDldNavigator::QueryPtr nav ( this->caller() );
  
  if ( nav.valid () )
  {
    nav->animate( false );
  }
#endif
}

void StopAnimation::_dummy ()
{
  USUL_TRACE_SCOPE;
  std::cout << "In Dummy" << std::endl;
}