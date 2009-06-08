
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdFasterAnimationSpeed.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdFasterAnimationSpeed );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdFasterAnimationSpeed::MpdFasterAnimationSpeed ( Usul::Interfaces::IUnknown * caller, unsigned int multiplier, const std::string &text ) :
  BaseClass ( caller ),
  _multiplier( multiplier )
{
  USUL_TRACE_SCOPE;
  this->text ( text );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdFasterAnimationSpeed::~MpdFasterAnimationSpeed ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdFasterAnimationSpeed::_execute ()
{
  USUL_TRACE_SCOPE;
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );
  if ( nav.valid () )
  {
    unsigned int speed = nav->getAnimationSpeed();
    speed = static_cast< unsigned int > ( speed / _multiplier );
    if( speed < 1 )
      speed = 1;

    nav->setAnimationSpeed( speed );
  }
#endif
}


