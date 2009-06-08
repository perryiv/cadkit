
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "MpdAnimationSpeed.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdAnimationSpeed );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdAnimationSpeed::MpdAnimationSpeed ( Usul::Interfaces::IUnknown * caller, unsigned int speed, const std::string &text ) :
  BaseClass ( caller ),
  _speed( speed )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdAnimationSpeed::~MpdAnimationSpeed ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdAnimationSpeed::_execute ()
{
  USUL_TRACE_SCOPE;
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );
  if ( nav.valid () )
  {
    nav->setAnimationSpeed( _speed );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the radio button.  Return true to have radio checked.
//
///////////////////////////////////////////////////////////////////////////////

bool MpdAnimationSpeed::updateCheck () const
{
  USUL_TRACE_SCOPE;

  // Implement me to update radio state.
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( const_cast < Usul::Interfaces::IUnknown * > ( this->caller().get() ) );

  unsigned int speed = nav->getAnimationSpeed();
  if( speed == _speed )
    return true;
  else
    return false;
}


