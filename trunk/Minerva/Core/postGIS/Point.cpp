
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
//   Build a node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Point::buildScene()
{
  return new osg::Node;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Point::buildGeometry()
{
  return new osg::Geometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the center of the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3 Point::getCenter ( float xOffset, float yOffset, float zOffset )
{
  VertexList vertexList ( _parser->getVertices() );

  if( vertexList.size() == 1 )
  {
    osg::Vec3 center;
    BinaryParser::Vertex v ( vertexList[0][0] );
    center.set( v[0], v[1], 0.0 );

    ossimMapProjection *mapProj = dynamic_cast < ossimMapProjection * > ( _projection.get() );

    if( this->_isSridSphereical( _srid ) )
    {
      Magrathea::convertToEarthCoordinates( center, zOffset );
    }
    else if( _projection.valid() && 0x0 != mapProj )
    {
      Magrathea::convertToEarthCoordinates( center, mapProj, zOffset );
    }

    return center;
  }

  return BaseClass::getCenter ( xOffset, yOffset, zOffset );
}
