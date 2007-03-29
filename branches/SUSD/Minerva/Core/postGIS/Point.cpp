
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Point.h"

#include "Magrathea/Project.h"

#include "osg/Node"
#include "osg/Geometry"

#include "ossim/projection/ossimMapProjection.h"

using namespace Minerva::Core::postGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Point, Point::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::Point ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F ) :
BaseClass( connection, tableName, id, srid, F )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Point::~Point()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Point::buildPointData( PrimitiveType type )
{
  return new osg::Geometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center of the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f Point::geometryCenter ( const osg::Vec3f& offset )
{
  const VertexList& vertexList ( this->_vertices() );

  if( vertexList.size() == 1 && vertexList[0].size() == 1 )
  {
    osg::Vec3 center;
    BinaryParser::Vertex v ( vertexList[0][0] );
    center.set( v[0], v[1], 0.0 );

    ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

    if( this->_isSridSphereical( _srid ) )
    {
      Magrathea::convertToEarthCoordinates( center, offset.z() );
    }
    else if( _projection.valid() && 0x0 != mapProj )
    {
      Magrathea::convertToEarthCoordinates( center, mapProj, offset.z() );
    }

    return center;
  }

  return BaseClass::geometryCenter ( offset );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Point::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IPointData::IID:
    return static_cast < Usul::Interfaces::IPointData* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
