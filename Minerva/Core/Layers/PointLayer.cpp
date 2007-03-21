
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Layers/PointLayer.h"
#include "Minerva/Core/postGIS/Point.h"
#include "Minerva/Core/DataObjects/Point.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "Serialize/XML/RegisterCreator.h"

#include <map>

using namespace Minerva::Core::Layers;

SERIALIZE_XML_REGISTER_CREATOR ( PointLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::PointLayer() : BaseClass(),
_primitiveID( 2 ),
_size( 5 ),
_stackPoints ( false )
{
  this->name( "PointLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::PointLayer ( const PointLayer& layer ) : BaseClass ( layer ),
_primitiveID( layer._primitiveID ),
_size( layer._size ),
_stackPoints ( layer._stackPoints )
{
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::_registerMembers()
{
  SERIALIZE_XML_ADD_MEMBER ( _primitiveID );
  SERIALIZE_XML_ADD_MEMBER ( _size );
  SERIALIZE_XML_ADD_MEMBER ( _stackPoints );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Layer* PointLayer::clone() const
{
  return new PointLayer( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::~PointLayer()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller )
{
  // Lock the mutex.
  Guard guard( _mutex );

  this->legendObject()->icon ( this->colorFunctor()->icon() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( caller );

  // Execute the query.
  pqxx::result geometryResult ( this->connection()->executeQuery ( this->query() ) );

  if( progress.valid() )
    progress->setTotalProgressBar( geometryResult.size() );

  // The data table.
  std::string dataTable ( this->tablename() );

  // Loop through the results.
  for ( pqxx::result::const_iterator iter = geometryResult.begin(); iter != geometryResult.end(); ++ iter )
  {
    // Get the id.
    int id ( iter["id"].as < int > () );
    int srid ( iter["srid"].as < int > () );

    Minerva::Core::postGIS::Geometry::RefPtr geometry ( new Minerva::Core::postGIS::Point ( this->connection(), this->tablename(), id, srid, iter["geom"] ) );
    geometry->zOffset( this->zOffset() );

    Minerva::Core::DataObjects::Point::RefPtr data ( new Minerva::Core::DataObjects::Point );
    data->geometry( geometry.get() );
    data->color( this->_color ( iter ) );
    data->size ( this->size() );
    data->primitiveId ( this->primitiveID() );
    data->renderBin( this->renderBin() );
    data->tableName( dataTable );
    data->rowId( id );

     /// Set the label.
    this->_labelDataObject( data.get() );

    data->buildScene();

    this->_addDataObject( data.get() );

    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }

  if( _stackPoints )
  {
    typedef osg::Vec3 Key;
    typedef Layer::DataObjects Value;
    typedef std::map< Key, Value > Map;

    Map map;

    DataObjects &dataObjects ( this->_getDataObjects() );

    for( Layer::DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
    {
      map[(*iter)->geometry()->getCenter()].push_back ( *iter );
    }

    for( Map::iterator iter = map.begin(); iter != map.end(); ++iter )
    {
      if( iter->second.size() > 1 )
      {
        Value& value ( iter->second );

        for( unsigned int i = 1; i < value.size(); ++i )
        {
          Minerva::Core::postGIS::Geometry::RefPtr geom ( value.at( i )->geometry() );
          float zOffset ( geom->zOffset() );
          geom->zOffset( zOffset + ( i * 20 ) );
          value.at( i )->dirty( true );
        }
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  DataObjects &dataObjects ( this->_getDataObjects() );
  dataObjects.clear();

  this->buildDataObjects ( caller );
/*
  DataObjects &dataObjects ( this->_getDataObjects() );

  for ( DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
  {
    Minerva::DataObjects::Point::RefPtr data ( dynamic_cast < Minerva::DataObjects::Point* > ( iter->get() ) );
    data->size ( this->size() );
    data->primitiveId ( this->primitiveID() );
    data->renderBin( this->renderBin() );
  }*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::primitiveID( Usul::Types::Uint32 primitiveId )
{
  _primitiveID = primitiveId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 PointLayer::primitiveID() const
{
  return _primitiveID;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::size( float size )
{
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayer::size() const
{
  return _size;
}


/// Set data members from given layer.
void PointLayer::setDataMembers ( Layer * layer )
{
  BaseClass::setDataMembers ( layer );

  if( PointLayer *point = dynamic_cast < PointLayer * > ( layer ) )
  {
    this->_primitiveID = point->_primitiveID;
    this->_size = point->_size;
    this->_stackPoints = point->_stackPoints;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::stackPoints( bool b )
{
  _stackPoints = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PointLayer::stackPoints() const
{
  return _stackPoints;
}
