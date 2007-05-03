
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Point.h"

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

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
//  Return the point
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Point::pointData( )
{
  Usul::Math::Vec3d point;

  const VertexList& vertexList ( this->_vertices() );

  if( vertexList.size() > 0 )
  {
    Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

    BinaryParser::Vertex v ( vertexList[0][0] );
    point.set( v[0] + _offset[0], v[1] + _offset[1], _offset[2] );

    if( project.valid() )
    {
      Usul::Math::Vec3d latLongPoint;
      project->projectToSpherical( point, _srid, latLongPoint );
      point[0] = latLongPoint[0];
      point[1] = latLongPoint[1];
      point[2] = latLongPoint[2];
    }
  }

  return point;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center of the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f Point::geometryCenter ( unsigned int& srid )
{
  return Point::geometryCenter( this->spatialOffset(), srid );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center of the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f Point::geometryCenter ( const osg::Vec3f& offset, unsigned int& srid )
{
  srid = this->srid();

  const VertexList& vertexList ( this->_vertices() );

  if( vertexList.size() == 1 && vertexList[0].size() == 1 )
  {
    osg::Vec3 center;
    BinaryParser::Vertex v ( vertexList[0][0] );
    center.set( v[0], v[1], offset.z() );

    return center;
  }

  return BaseClass::geometryCenter ( offset, srid );
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
