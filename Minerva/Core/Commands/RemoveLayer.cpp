
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
#include "Usul/Interfaces/IDocument.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( RemoveLayer, RemoveLayer::BaseClass );
USUL_IMPLEMENT_COMMAND ( RemoveLayer );
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

RemoveLayer::RemoveLayer ( Usul::Interfaces::IUnknown* layer ) : 
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

void RemoveLayer::_execute()
{
  // Use the caller if we have one, if not fall back on the active document.
  Usul::Interfaces::IUnknown::QueryPtr caller ( this->caller().get() );
  Usul::Interfaces::IUnknown::QueryPtr unknown ( true == caller.valid() ? caller : Usul::Documents::Manager::instance().activeDocument() );
  
  // Query for the interface.
  Minerva::Interfaces::IRemoveLayer::QueryPtr remove ( unknown );

  // Remove the layer.
  if ( remove.valid () )
    remove->removeLayer ( _layer );
  
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );
  if ( document.valid() )
  {
    document->requestRedraw();
    document->modified ( true );
  }
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
