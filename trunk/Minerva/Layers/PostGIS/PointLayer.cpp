
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Layers/PostGIS/PointLayer.h"
#include "Minerva/Layers/PostGIS/BinaryParser.h"

#include "Minerva/Core/DataObjects/Point.h"
#include "Minerva/Core/Visitor.h"
#include "Minerva/Interfaces/IOffset.h"

#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Trace/Trace.h"

#include "pqxx/pqxx"

#include <map>

using namespace Minerva::Layers::PostGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( PointLayer, PointLayer::BaseClass );
USUL_FACTORY_REGISTER_CREATOR ( PointLayer );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::PointLayer() : 
  BaseClass(),
  _primitiveID( 2 ),
  _size( 5 ),
  _secondarySize ( 0.00005 ),
  _stackPoints ( false ),
  _quality ( 0.80 ),
  _primitiveSizeColumn(),
  _autotransform ( true )
{
  USUL_TRACE_SCOPE;
  this->name( "PointLayer" );

  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PointLayer::PointLayer ( const PointLayer& layer ) : 
  BaseClass ( layer ),
  _primitiveID( layer._primitiveID ),
  _size( layer._size ),
  _secondarySize ( layer._secondarySize ),
  _stackPoints ( layer._stackPoints ),
  _quality( layer._quality ),
  _primitiveSizeColumn( layer._primitiveSizeColumn ),
  _autotransform ( layer._autotransform )
{
  USUL_TRACE_SCOPE;
  this->_registerMembers();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register members.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::_registerMembers()
{
  USUL_TRACE_SCOPE;
  SERIALIZE_XML_ADD_MEMBER ( _primitiveID );
  SERIALIZE_XML_ADD_MEMBER ( _size );
  SERIALIZE_XML_ADD_MEMBER ( _secondarySize );
  SERIALIZE_XML_ADD_MEMBER ( _stackPoints );
  SERIALIZE_XML_ADD_MEMBER ( _quality );
  SERIALIZE_XML_ADD_MEMBER ( _primitiveSizeColumn );
  SERIALIZE_XML_ADD_MEMBER ( _autotransform );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Accept the visitor.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::accept ( Minerva::Core::Visitor& visitor )
{
  USUL_TRACE_SCOPE;
  visitor.visit ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone this layer.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointLayer::clone() const
{
  USUL_TRACE_SCOPE;
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
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the data objects.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::buildDataObjects( Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *p )
{
  USUL_TRACE_SCOPE;
  Connection::ScopedConnection scopedConnection ( *this->connection() );

  Usul::Interfaces::IProgressBar::QueryPtr progress ( p );

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

      pqxx::binarystring buffer ( iter["geom"] );
      BinaryParser parser;
      BinaryParser::Geometries geometries ( parser ( &buffer.front() ) );

      for ( BinaryParser::Geometries::iterator geom = geometries.begin(); geom != geometries.end(); ++geom )
      {
        (*geom)->srid( srid );
        Usul::Interfaces::IUnknown::QueryPtr unknown ( *geom );
        Minerva::Interfaces::IOffset::QueryPtr offset ( unknown );

        if( offset.valid () )
        {
          offset->spatialOffset( osg::Vec3f ( this->xOffset(), this->yOffset(), this->zOffset() ) );
        }

        Minerva::Core::DataObjects::Point::RefPtr data ( new Minerva::Core::DataObjects::Point );
        data->geometry( unknown.get() );
        data->color( this->_color ( iter ) );
        data->size ( this->size() );
        data->primitiveId ( this->primitiveID() );
        data->quality ( this->quality() );
        data->objectId ( Usul::Strings::format ( id ) );
        data->autotransform ( this->autotransform () );
        data->secondarySize ( this->secondarySize() );

        if( this->primitiveSizeColumn().size() > 0 )
        {
          float value ( iter [ this->primitiveSizeColumn() ].as < float > () );
          data->size( this->size() * value );
          this->_updateMinMax( value );
        }

         /// Set the common members.
        this->_setDataObjectMembers( data.get(), caller );

        // Pre build the scene.
        data->preBuildScene( caller );

        this->add ( Usul::Interfaces::IUnknown::QueryPtr ( data.get() ) );
      }
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Modify the layer.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::modify( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  
  // For now get what we have, clear and then rebuild.
  // Need a way to tell if the query has changed.  Then I think this can be handled better.
  this->clear();
  this->buildDataObjects ( caller, 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::primitiveID( Usul::Types::Uint32 primitiveId )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _primitiveID = primitiveId;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive id.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Types::Uint32 PointLayer::primitiveID() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _primitiveID;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::size( float size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _size = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the size.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayer::size() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the secondary size.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayer::secondarySize () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _secondarySize;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the secondary size.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::secondarySize ( float size )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _secondarySize = size;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::stackPoints( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _stackPoints = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the stack points flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PointLayer::stackPoints() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _stackPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* PointLayer::queryInterface( unsigned long iid )
{
  USUL_TRACE_SCOPE;
  
  switch ( iid )
  {
  case Minerva::Interfaces::IPointLayer::IID:
    return static_cast < Minerva::Interfaces::IPointLayer* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::quality( float value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _quality = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

float PointLayer::quality() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _quality;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set quality.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::primitiveSizeColumn( const std::string& value )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _primitiveSizeColumn = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get quality.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& PointLayer::primitiveSizeColumn() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _primitiveSizeColumn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the default query.
//
///////////////////////////////////////////////////////////////////////////////

std::string PointLayer::defaultQuery() const
{
  USUL_TRACE_SCOPE;
  
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


///////////////////////////////////////////////////////////////////////////////
//
//  Set use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

void PointLayer::autotransform ( bool b )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  _autotransform = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get use auto transform flag.
//
///////////////////////////////////////////////////////////////////////////////

bool PointLayer::autotransform () const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _autotransform;
}

