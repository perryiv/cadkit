
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

#include "Minerva/Core/Data/DataObject.h"
#include "Minerva/Core/Geometry/Point.h"
#include "Minerva/Core/Visitor.h"

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

void PointLayer::_setGeometryMembers( Geometry* geometry, const pqxx::result::const_iterator& iter )
{
  USUL_TRACE_SCOPE;
  typedef Minerva::Core::Geometry::Point Point;
  
  // Set the point style.
  if ( Point* point = dynamic_cast<Point*> ( geometry ) )
  {
    point->size ( this->size() );
    point->primitiveId ( this->primitiveID() );
    point->quality ( this->quality() );
    point->autotransform ( this->autotransform() );
    point->secondarySize ( this->secondarySize() );

    if( this->primitiveSizeColumn().size() > 0 )
    {
      const float value ( iter [ this->primitiveSizeColumn() ].as < float > () );
      point->size( this->size() * value );
      this->_updateMinMax( value );
    }
  }
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
    query << ", " << this->colorColumn();

  if ( this->primitiveSizeColumn().size() > 0 )
    query << ", " << this->primitiveSizeColumn();
  
  if ( this->firstDateColumn().size() > 0 )
    query << ", " << this->firstDateColumn();
  
  if ( this->lastDateColumn().size() > 0 )
    query << ", " + this->lastDateColumn();
  
  query << " FROM " << this->tablename( );
  
  std::string whereClause ( this->_whereClause() );
  
  if ( whereClause.size() > 0 )
    query << " WHERE " << whereClause;
  
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

