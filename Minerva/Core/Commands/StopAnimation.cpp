
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/StopAnimation.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( StopAnimation, StopAnimation::BaseClass );
USUL_IMPLEMENT_COMMAND ( StopAnimation );
USUL_FACTORY_REGISTER_CREATOR ( StopAnimation );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StopAnimation::StopAnimation ( ) : 
  BaseClass( 0x0 )
{
  this->text ( "Stop Animation" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StopAnimation::StopAnimation ( Usul::Interfaces::IUnknown *caller ) : 
  BaseClass( caller )
{
  this->text ( "Stop Animation" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StopAnimation::~StopAnimation()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void StopAnimation::_execute ()
{
  Minerva::Interfaces::IAnimationControl::QueryPtr control ( this->caller() );

  if ( control.valid() )
    control->stopAnimation();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* StopAnimation::queryInterface( unsigned long iid )
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
