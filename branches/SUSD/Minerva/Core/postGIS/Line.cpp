
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Line.h"

#include "Magrathea/Project.h"
#include "Magrathea/Resample.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Minerva::Core::postGIS;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F ) :
BaseClass( connection, tableName, id, srid, F ),
_points()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::~Line()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Line::buildScene()
{
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );
  geode->addDrawable( this->buildGeometry() );
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Line::buildGeometry()
{
  if( _points.empty() )
    this->_buildLatLongPoints();

  // Create the geometry
  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  unsigned int num ( 0 );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

  for( PointsList::iterator iter = _points.begin(); iter != _points.end(); ++iter )
  {
    LatLongPoints sampledPoints;
    Magrathea::resample( *iter, sampledPoints );

    osg::ref_ptr< osg::Vec3Array > newVertices ( new osg::Vec3Array );
    Magrathea::convertVerticesToEarthCoordinates( sampledPoints, *newVertices, this->zOffset() );

    vertices->insert( vertices->end(), newVertices->begin(), newVertices->end() );

    // Add the primitive set
    geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP, num, newVertices->size() ) );

    num += newVertices->size();
  }

  // Set the vertices.
	geometry->setVertexArray( vertices.get() );

  return geometry.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_buildLatLongPoints()
{
  _points.clear();

  VertexList vertexList ( _parser->getVertices() );

  for( VertexList::iterator iter = vertexList.begin(); iter != vertexList.end(); ++iter )
  {
    LatLongPoints latLongPoints;
    this->_convertToLatLong( *iter, latLongPoints );
    _points.push_back ( latLongPoints );
  }
}
