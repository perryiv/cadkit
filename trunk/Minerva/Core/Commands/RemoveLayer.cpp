
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/RemoveLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( RemoveLayer, RemoveLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( RemoveLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RemoveLayer::RemoveLayer ( ) : 
  BaseClass( 0x0 )
{
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RemoveLayer::RemoveLayer ( Usul::Interfaces::ILayer* layer ) : 
  BaseClass( 0x0 ),
  _layer ( layer )
{
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RemoveLayer::~RemoveLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void RemoveLayer::_execute ()
{
  Minerva::Interfaces::IRemoveLayer::QueryPtr remove ( Usul::Documents::Manager::instance().active () );

  // Remove the layer.
  if ( remove.valid () )
    remove->removeLayer ( _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* RemoveLayer::queryInterface( unsigned long iid )
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
