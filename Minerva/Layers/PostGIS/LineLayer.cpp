
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/LineLayer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Core/Visitor.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "pqxx/pqxx"

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( LineLayer, LineLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( LineLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer() : BaseClass(),
_lineWidth ( 1.0f )
{
  this->name( "LineLayer" );
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::LineLayer ( const LineLayer& layer ) : BaseClass ( layer ),
_lineWidth ( layer._lineWidth )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _lineWidth );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::accept ( Minerva::Core::Visitor& visitor )
{
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LineLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy ( new LineLayer( *this ) );
  return copy.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

LineLayer::~LineLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line width.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::lineWidth( float width )
{
  Guard guard( this->mutex() );
  _lineWidth = width;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the line width.
//
///////////////////////////////////////////////////////////////////////////////

float LineLayer::lineWidth() const
{
  Guard guard( this->mutex() );
  return _lineWidth;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *p )
{
  Connection::ScopedConnection scopedConnection ( *this->connection() );

  // Query for a progress bar.
  Usul::Interfaces::IProgressBar::QueryPtr progress ( p );

  // Execute the query.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  // Set the total number of items.
  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  // Data needed below.
  const std::string dataTable ( this->tablename() );
  const osg::Vec3f offset ( 0.0, 0.0, this->zOffset() );
  const float lineWidth ( this->lineWidth() );

  // Loop through the results.
  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    // Get the id.
    int id ( iter["id"].as < int > () );
    int srid ( iter["srid"].as < int > () );

    pqxx::binarystring buffer ( iter["geom"] );
    BinaryParser parser;
    BinaryParser::Geometries geometries ( parser ( &buffer.front() ) );

    for ( BinaryParser::Geometries::iterator geom = geometries.begin(); geom != geometries.end(); ++geom )
    {
      (*geom)->srid( srid );
      Usul::Interfaces::IUnknown::QueryPtr unknown ( *geom );
      Minerva::Interfaces::IOffset::QueryPtr sp ( geom->get() );

      if( sp.valid() )
        sp->spatialOffset( offset );

      Minerva::Core::DataObjects::Line::RefPtr data ( new Minerva::Core::DataObjects::Line( ) );
      data->color ( this->_color ( iter ) );
      data->width( lineWidth );
      data->geometry( unknown.get() );
      data->objectId ( Usul::Strings::format ( id ) );

      // Set the label.
      this->_setDataObjectMembers( data.get(), caller );

      // Build the scene.
      data->preBuildScene( caller );

      // Add the data object.
      this->addDataObject( data.get() );
    }

    // Update progress.
    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify layer.
//
///////////////////////////////////////////////////////////////////////////////

void LineLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  this->clearDataObjects();
  this->buildDataObjects ( caller, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* LineLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Minerva::Interfaces::ILineLayer::IID:
    return static_cast < Minerva::Interfaces::ILineLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
