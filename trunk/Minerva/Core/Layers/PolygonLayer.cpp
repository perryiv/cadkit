
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PolygonLayer.h"
#include "Minerva/Core/postGIS/Polygon.h"
#include "Minerva/Core/DataObjects/Polygon.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "osg/Group"

#include <algorithm>

using namespace Minerva::Core::Layers;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer() : BaseClass(), _format()
{
  this->name( "PolygonLayer" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::PolygonLayer ( const PolygonLayer& layer ) : BaseClass ( layer ),
_format ( layer._format )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer* PolygonLayer::clone() const
{
  return new PolygonLayer( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PolygonLayer::~PolygonLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  // Guard this section of code.
  Guard guard ( _mutex);

  this->legendObject()->icon( this->colorFunctor()->icon() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  std::string dataTable ( this->tablename() );

  // Execute the query.  This will return data to draw.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    // Id for row
    int id ( iter["id"].as < int > () );
    int srid ( iter["srid"].as < int > () );

    Minerva::Core::postGIS::Geometry::RefPtr geometry ( new Minerva::Core::postGIS::Polygon ( this->connection(), dataTable, id, srid, iter["geom"] ) );
    geometry->zOffset( this->zOffset() );

    Minerva::Core::DataObjects::Polygon::RefPtr data ( new Minerva::Core::DataObjects::Polygon );

    data->geometry ( geometry.get() );
    data->color( this->_color ( iter ) );
    data->renderBin( this->renderBin() );
    data->tableName ( dataTable );
    data->rowId ( id );

    this->_addDataObject( data.get() );

    data->buildScene();
    

    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  DataObjects &dataObjects ( this->_getDataObjects() );
  dataObjects.clear();

  this->buildDataObjects ( caller );

  // Guard this section of code.
  //Guard guard ( _mutex);

  //this->legendObject()->icon( this->colorFunctor()->icon() );

  //Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  //DataObjects &dataObjects ( this->_getDataObjects() );

  //if( progress.valid() )
  //  progress->setTotalProgressBar( dataObjects.size() );

  //for ( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
  //{
  //  // Update the render bin.
  //  (*iter)->renderBin( this->renderBin() );

  //  std::ostringstream query;
  //  query << "SELECT " << this->colorColumn() << " FROM " << this->tablename() << " WHERE id=" << (*iter)->rowId();
  //  pqxx::result result ( this->connection()->executeQuery ( query.str() ) );

  //  if( !result.empty() )
  //    (*iter)->color ( this->_color( result.begin() ) );

  //  if( progress.valid() )
  //  {
  //    unsigned int num ( iter - dataObjects.begin() );
  //    progress->updateProgressBar( num );
  //  }
  //}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the format string.
//
///////////////////////////////////////////////////////////////////////////////

void PolygonLayer::format( const std::string& format )
{
  _format = format;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the format string.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PolygonLayer::format() const
{
  return _format;
}

/// Set data members from given layer.
void PolygonLayer::setDataMembers ( Layer * layer )
{
  BaseClass::setDataMembers ( layer );

  if( PolygonLayer *polygon = dynamic_cast < PolygonLayer * > ( layer ) )
  {
    this->_format = polygon->_format;
  }
}
