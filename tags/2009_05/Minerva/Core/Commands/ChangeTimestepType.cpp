
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/ChangeTimestepType.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Documents/Manager.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ChangeTimestepType, ChangeTimestepType::BaseClass );
USUL_IMPLEMENT_COMMAND ( ChangeTimestepType );
USUL_FACTORY_REGISTER_CREATOR ( ChangeTimestepType );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChangeTimestepType::ChangeTimestepType ( ) : 
  BaseClass ( 0x0 ),
  _type    ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_DAY )
{
  this->_addMember ( "type", reinterpret_cast < unsigned int& > ( _type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ChangeTimestepType::ChangeTimestepType ( TimestepType type, Usul::Interfaces::IUnknown * caller ) : 
  BaseClass ( caller ),
  _type    ( type )
{
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_SECOND == type )
    this->text ( "Second" );
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_MINUTE == type )
    this->text ( "Minute" );
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_HOUR == type )
    this->text ( "Hour" );
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_DAY == type )
    this->text ( "Day" );
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_MONTH == type )
    this->text ( "Month" );
  if ( Minerva::Interfaces::IAnimationControl::TIMESTEP_TYPE_YEAR == type )
    this->text ( "Year" );

  this->_addMember ( "type", reinterpret_cast < unsigned int& > ( _type ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ChangeTimestepType::~ChangeTimestepType()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Attempt to change the animation step type.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  bool tryTimestepTypeChange ( Usul::Interfaces::IUnknown *caller, Minerva::Interfaces::IAnimationControl::TimestepType type )
  {
    Minerva::Interfaces::IAnimationControl::QueryPtr control ( caller );

    if ( control.valid() )
    {
      control->timestepType ( type );
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

void ChangeTimestepType::_execute ()
{
  Guard guard ( this );

  if ( false == Detail::tryTimestepTypeChange ( this->caller (), _type ) )
    Detail::tryTimestepTypeChange ( Usul::Documents::Manager::instance().activeDocument (), _type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ChangeTimestepType::queryInterface( unsigned long iid )
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


///////////////////////////////////////////////////////////////////////////////
//
//  Should the button be checked?
//
///////////////////////////////////////////////////////////////////////////////

bool ChangeTimestepType::updateCheck() const
{
  Guard guard ( this );
  Minerva::Interfaces::IAnimationControl::QueryPtr ac ( const_cast < Usul::Interfaces::IUnknown* > ( this->caller().get() ) );
  return ( ac.valid() ? _type == ac->timestepType() : false );
}
