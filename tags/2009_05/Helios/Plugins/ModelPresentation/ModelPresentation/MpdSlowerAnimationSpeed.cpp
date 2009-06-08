
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/ModelPresentation/ModelPresentation/MpdSlowerAnimationSpeed.h"

#include "Usul/Trace/Trace.h"
#include "Usul/Interfaces/IMpdNavigator.h"

USUL_IMPLEMENT_COMMAND ( MpdSlowerAnimationSpeed );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

MpdSlowerAnimationSpeed::MpdSlowerAnimationSpeed ( Usul::Interfaces::IUnknown * caller, unsigned int multiplier, const std::string &text ) :
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

MpdSlowerAnimationSpeed::~MpdSlowerAnimationSpeed ()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute.
//
///////////////////////////////////////////////////////////////////////////////

void MpdSlowerAnimationSpeed::_execute ()
{
  USUL_TRACE_SCOPE;
#if 1
  Usul::Interfaces::IMpdNavigator::QueryPtr nav ( this->caller().get() );
  if ( nav.valid () )
  {
    unsigned int speed = nav->getAnimationSpeed();
    speed *= _multiplier;

    nav->setAnimationSpeed( speed );
  }
#endif
}


