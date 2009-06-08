
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/ShowLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Factory/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ShowLayer, ShowLayer::BaseClass );
USUL_IMPLEMENT_COMMAND ( ShowLayer );
USUL_FACTORY_REGISTER_CREATOR ( ShowLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowLayer::ShowLayer() : 
  BaseClass( 0x0 )
{
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShowLayer::ShowLayer ( Usul::Interfaces::ILayer* layer ) : 
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

ShowLayer::~ShowLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void ShowLayer::_execute()
{
  Minerva::Interfaces::IDirtyScene::QueryPtr dirty ( Usul::Documents::Manager::instance().activeDocument() );
  
  // Hide the layer.
  if ( _layer.valid() )
    _layer->showLayer ( true );

  // Dirty the scene.
  if ( dirty.valid() )
    dirty->dirtyScene ( true );
  
  Usul::Interfaces::IDocument::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );
  if ( document.valid() )
    document->requestRedraw();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* ShowLayer::queryInterface( unsigned long iid )
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
