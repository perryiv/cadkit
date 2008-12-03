
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Commands/AddLayer.h"
#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IDirtyScene.h"

#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AddLayer, AddLayer::BaseClass );
USUL_IMPLEMENT_COMMAND ( AddLayer );
USUL_FACTORY_REGISTER_CREATOR ( AddLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer() : 
  BaseClass( 0x0 ),
  _parent (),
  _layer ()
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer ( Usul::Interfaces::IUnknown * parent, Usul::Interfaces::IUnknown* layer ) : 
  BaseClass( 0x0 ),
  _parent ( parent ),
  _layer ( layer )
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::~AddLayer()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::_execute()
{
  USUL_TRACE_SCOPE;

  Minerva::Interfaces::IAddLayer::QueryPtr addLayer ( _parent );
  
  // Add the layer.
  if( addLayer.valid () )
  {
    addLayer->addLayer ( _layer );
  
    // Dirty the scene.
    Minerva::Interfaces::IDirtyScene::QueryPtr ds ( Usul::Documents::Manager::instance().activeDocument() );
    if ( ds.valid() )
      ds->dirtyScene ( true, _layer );
  
    // Request a redraw of the active document.
    Usul::Interfaces::IDocument::QueryPtr document ( Usul::Documents::Manager::instance().activeDocument() );
    if ( document.valid() )
    {
      document->requestRedraw();
      document->modified ( true );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* AddLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
