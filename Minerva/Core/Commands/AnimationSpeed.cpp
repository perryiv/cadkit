
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/AnimationSpeed.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AnimationSpeed, AnimationSpeed::BaseClass );
USUL_IMPLEMENT_COMMAND ( AnimationSpeed );
USUL_FACTORY_REGISTER_CREATOR ( AnimationSpeed );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationSpeed::AnimationSpeed ( ) : 
  BaseClass ( 0x0 ),
  _speed    ( 0.0 )
{
  this->_addMember ( "speed", _speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationSpeed::AnimationSpeed ( double speed ) : 
  BaseClass ( 0x0 ),
  _speed    ( speed )
{
  this->_addMember ( "speed", _speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AnimationSpeed::~AnimationSpeed()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationSpeed::_execute ()
{
  Minerva::Interfaces::IAnimationControl::QueryPtr control ( this->caller() );

  if ( control.valid() )
    control->animateSpeed ( _speed );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* AnimationSpeed::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}

