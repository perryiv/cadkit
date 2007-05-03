
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
#include "Usul/Interfaces/IOffset.h"

#include <map>

using namespace Minerva::Core::Layers;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PointLayer, PointLayer::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::PointLayer() : BaseClass(),
_primitiveID( 2 ),
_size( 5 ),
_stackPoints ( false ),
_quality ( 0.80 ),
_primitiveSizeColumn()
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
_stackPoints ( layer._stackPoints ),
_quality( layer._quality ),
_primitiveSizeColumn( layer._primitiveSizeColumn )
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
  SERIALIZE_XML_ADD_MEMBER ( _quality );
  SERIALIZE_XML_ADD_MEMBER ( _primitiveSizeColumn );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointLayer::clone() const
{
  Usul::Interfaces::IUnknown::QueryPtr copy ( new PointLayer( *this ) );
  return copy.release();
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
    try
    {
      // Get the id.
      int id ( iter["id"].as < int > () );
      int srid ( iter["srid"].as < int > () );

      Usul::Interfaces::IUnknown::QueryPtr unknown ( new Minerva::Core::postGIS::Point ( this->connection(), this->tablename(), id, srid, iter["geom"] ) );

      Usul::Interfaces::IOffset::QueryPtr offset ( unknown );

      if( offset.valid () )
      {
        offset->spatialOffset( osg::Vec3f ( this->xOffset(), this->yOffset(), this->zOffset() ) );
      }

      Minerva::Core::DataObjects::Point::RefPtr data ( new Minerva::Core::DataObjects::Point );
      data->geometry( unknown->queryInterface( Usul::Interfaces::IUnknown::IID ) );
      data->color( this->_color ( iter ) );
      data->size ( this->size() );
      data->primitiveId ( this->primitiveID() );
      data->quality ( this->quality() );
      data->renderBin( this->renderBin() );
      data->connection ( this->connection() );
      data->tableName ( dataTable );
      data->rowId ( id );

      if( this->primitiveSizeColumn().size() > 0 )
      {
        float value ( iter [ this->primitiveSizeColumn() ].as < float > () );
        data->size( this->size() * value );
      }

       /// Set the label.
      this->_labelDataObject( data.get() );

      this->_addDataObject( data.get() );
    }
    catch ( const std::exception& e )
    {
      std::cout << "Error 6442903120: " << e.what() << std::endl;
    }
    catch ( ... )
    {
      std::cout << "Error 1112177078: Exception caught while adding data to layer." << std::endl;
    }

    if( progress.valid() )
    {
      unsigned int num ( iter - geometryResult.begin() );
      progress->updateProgressBar( num );
    }
  }

  /// Stack the points.
  this->_stack();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Stack the points.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::_stack()
{
  if( _stackPoints )
  {
    typedef osg::Vec3 Key;
    typedef Layer::DataObjects Value;
    typedef std::map< Key, Value > Map;

    Map map;

    DataObjects &dataObjects ( this->_getDataObjects() );

    unsigned int srid ( 0 );

    for( Layer::DataObjects::iterator iter = dataObjects.begin(); iter != dataObjects.end(); ++iter )
    {
      Usul::Interfaces::IGeometryCenter::QueryPtr geometryCenter ( (*iter)->geometry() );

      if( geometryCenter.valid () )
      {
        map[ geometryCenter->geometryCenter( srid ) ].push_back ( *iter );
      }
    }

    for( Map::iterator iter = map.begin(); iter != map.end(); ++iter )
    {
      if( iter->second.size() > 1 )
      {
        Value& value ( iter->second );

        for( unsigned int i = 1; i < value.size(); ++i )
        {
          Usul::Interfaces::IOffset::QueryPtr offset ( value.at( i )->geometry() );

          if( offset.valid () )
          {
            const osg::Vec3& o ( offset->spatialOffset() );
            offset->spatialOffset ( osg::Vec3f ( o.x(), o.y(), o.z() + ( i * 20 ) ) );
            value.at( i )->dirty( true );
          }
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


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointLayer::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  //case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPointLayer::IID:
    return static_cast < Usul::Interfaces::IPointLayer* > ( this );
  case Minerva::Core::IPointLayerRawPointer::IID:
    return static_cast < Minerva::Core::IPointLayerRawPointer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point layer.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer* PointLayer::getRawPointer()
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the point layer.
//
///////////////////////////////////////////////////////////////////////////////

const PointLayer* PointLayer::getRawPointer() const
{
  return this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::quality( float value )
{
  _quality = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayer::quality() const
{
  return _quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::primitiveSizeColumn( const std::string& value )
{
  _primitiveSizeColumn = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointLayer::primitiveSizeColumn() const
{
  return _primitiveSizeColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default query.
//
///////////////////////////////////////////////////////////////////////////////

std::string PointLayer::defaultQuery() const
{
  // Get the geometry colomn.
  const std::string geomColumn ( this->geometryColumn() );

  // Build the query.
  std::ostringstream query;
  query << "SELECT " << this->primaryKeyColumn() << " as id, srid(" << geomColumn << ") as srid, asBinary(" << geomColumn << ") as geom";
  if ( this->colorColumn().size() > 0 )
  {
    query << ", " << this->colorColumn();
  }

  if ( this->primitiveSizeColumn().size() > 0 )
  {
    query << ", " << this->primitiveSizeColumn();
  }
  
  query << " FROM " << this->tablename( );
  return query.str();
}
