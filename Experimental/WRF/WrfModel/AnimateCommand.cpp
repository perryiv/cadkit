
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "AnimateCommand.h"

#include "Usul/Trace/Scope.h"
#include "Usul/Interfaces/ITimestepAnimation.h"

USUL_IMPLEMENT_COMMAND ( AnimateCommand );

AnimateCommand::AnimateCommand ( bool animate, Usul::Interfaces::IUnknown * caller ) :
  BaseClass ( caller ),
  _animate ( animate )
{
  this->text ( animate ? "Start Animation" : "Stop Animation" );
}


AnimateCommand::~AnimateCommand ()
{
}

void AnimateCommand::_execute ()
{
  Usul::Interfaces::ITimestepAnimation::QueryPtr animate ( this->caller () );

  if ( animate.valid() )
  {
    if ( _animate )
      animate->startTimestepAnimation ();
    else
      animate->stopTimestepAnimation ();
  }
}

