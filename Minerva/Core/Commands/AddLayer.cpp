
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

#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IVectorLayer.h"

#include "Serialize/XML/RegisterCreator.h"

using namespace Minerva::Core::Commands;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( AddLayer, AddLayer::BaseClass );
SERIALIZE_XML_REGISTER_CREATOR ( AddLayer );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayer ( ) : 
  BaseClass( 0x0 )
{
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
  this->_addMember ( "layer", _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::~AddLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute command.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::_execute ()
{
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
  // Capture the active document.
  _document = Usul::Documents::Manager::instance().active();

  Minerva::Interfaces::IAddLayer::QueryPtr addLayer ( _document );

  // Add the layer.
  if( addLayer.valid () )
    addLayer->addLayer ( _layer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AddLayer::AddLayerJob::~AddLayerJob()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects and add the layer to the scene.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::AddLayerJob::_started()
{
  // Query for needed interfaces.
  Usul::Interfaces::IVectorLayer::QueryPtr vector ( _layer );

  // Build the vector data.
  if ( vector.valid () )
    vector->buildVectorData ( this->progress() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.  
//  The destructor may be too late since we don't know when the object is deleted.
//
///////////////////////////////////////////////////////////////////////////////

void AddLayer::AddLayerJob::_finished()
{
  // No longer needed.  Unref.
  _layer = static_cast < Usul::Interfaces::ILayer *> ( 0x0 );
  _document = static_cast < Usul::Interfaces::IUnknown * > ( 0x0 );
}
