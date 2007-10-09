
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
#include "Usul/Documents/Manager.h"

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


namespace Detail
{
  bool tryAnimationSpeedChange ( Usul::Interfaces::IUnknown *caller, double speed )
  {
    Minerva::Interfaces::IAnimationControl::QueryPtr control ( caller );

    if ( control.valid() )
    {
      control->animateSpeed ( speed );
      return true;
    }

    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void AnimationSpeed::_execute ()
{
  if ( false == Detail::tryAnimationSpeedChange ( this->caller (), _speed ) )
    Detail::tryAnimationSpeedChange ( Usul::Documents::Manager::instance().activeDocument (), _speed );
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

