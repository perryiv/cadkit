
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/HideLayer.h"

#include "Minerva/Interfaces/IDirtyScene.h"

#include "Usul/Documents/Manager.h"

#include "Serialize/XML/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( HideLayer, HideLayer::BaseClass );
SERIALIZE_XML_REGISTER_CREATOR ( HideLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

HideLayer::HideLayer ( ) : 
  BaseClass( 0x0 )
{
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

  HideLayer::HideLayer ( Usul::Interfaces::ILayer* layer ) : 
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

HideLayer::~HideLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void HideLayer::_execute ()
{
  Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( Usul::Documents::Manager::instance().active () );
  
  // Hide the layer.
  if ( _layer.valid () )
    _layer->showLayer ( false );

  // Dirty the scene.
  if ( dirty.valid () )
    dirty->dirtyScene ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* HideLayer::queryInterface( unsigned long iid )
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
