
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
#include "Usul/Interfaces/IVectorLayer.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Trace/Trace.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AddLayer, AddLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( AddLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer ( ) : 
  BaseClass( 0x0 )
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer ( Usul::Interfaces::IUnknown * caller, Usul::Interfaces::ILayer* layer ) : 
  BaseClass( caller ),
  _layer ( layer )
{
  USUL_TRACE_SCOPE;
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer ( Usul::Interfaces::ILayer* layer ) : 
  BaseClass( 0x0 ),
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

void AddLayer::_execute ()
{
  USUL_TRACE_SCOPE;

  // Create a job to add a layer.  It captures the active document.
  AddLayerJob::RefPtr job ( new AddLayerJob ( _layer, this->caller() ) );
  Usul::Jobs::Manager::instance().add ( job.get() );
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
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::ISerialize::IID:
    return static_cast < Usul::Interfaces::ISerialize* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayerJob::AddLayerJob ( Usul::Interfaces::ILayer* layer, Usul::Interfaces::IUnknown *caller ) :
  BaseClass ( caller ),
  _layer ( layer ),
  _caller ( caller ),
  _document ( )
{
  USUL_TRACE_SCOPE;

  // Capture the active document.
  _document = Usul::Documents::Manager::instance().active();

  // Try adding the layer with the caller first.  If that fails, try the active document.
  if ( false == this->_addLayer ( caller ) )
    this->_addLayer ( _document );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayerJob::~AddLayerJob()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the layer.
//
///////////////////////////////////////////////////////////////////////////////

bool AddLayer::AddLayerJob::_addLayer ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  Minerva::Interfaces::IAddLayer::QueryPtr addLayer ( caller );

  // Add the layer.
  if( addLayer.valid () )
  {
    addLayer->addLayer ( _layer );
    _caller = caller;
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects and add the layer to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::AddLayerJob::_started()
{
  USUL_TRACE_SCOPE;

  // Query for needed interfaces.
  Usul::Interfaces::IVectorLayer::QueryPtr vector ( _layer );

  // Show the progess bar.  Hides in the destructor.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( this->progress () );

  // Build the vector data.
  if ( vector.valid () )
    vector->buildVectorData ( this->progress() );

  Minerva::Interfaces::IDirtyScene::QueryPtr ds ( _caller );
  if ( ds.valid ( ) )
    ds->dirtyScene ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.  
//  The destructor may be too late since we don't know when the object is deleted.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::AddLayerJob::_finished()
{
  USUL_TRACE_SCOPE;

  // No longer needed.  Unref.
  _layer = static_cast < Usul::Interfaces::ILayer *> ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}
