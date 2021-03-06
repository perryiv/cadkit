
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/ShowPastEvents.h"
#include "Minerva/Interfaces/IAnimationControl.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ShowPastEvents, ShowPastEvents::BaseClass );
USUL_IMPLEMENT_COMMAND ( ShowPastEvents );
USUL_FACTORY_REGISTER_CREATOR ( ShowPastEvents );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowPastEvents::ShowPastEvents ( ) : 
  BaseClass( 0x0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

  ShowPastEvents::ShowPastEvents ( Usul::Interfaces::IUnknown* unknown ) : 
  BaseClass( unknown )
{
  this->text ( "Show Past Events" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowPastEvents::~ShowPastEvents()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void ShowPastEvents::_execute()
{
  Minerva::Interfaces::IAnimationControl::QueryPtr ac ( this->caller().get() );
  
  // Toggle shown.
  if ( ac.valid() )
    ac->showPastEvents ( !ac->showPastEvents()  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ShowPastEvents::queryInterface( unsigned long iid )
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
//  Is the item checked?
//
///////////////////////////////////////////////////////////////////////////////

bool ShowPastEvents::updateCheck() const
{
  Minerva::Interfaces::IAnimationControl::QueryPtr ac ( const_cast < Usul::Interfaces::IUnknown* > ( this->caller().get() ) );
  return ac.valid() ? ac->showPastEvents() : false;
}
