
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

#include "Usul/Components/Manager.h"
#include "Usul/Interfaces/IProjectCoordinates.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Minerva::Core::postGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Line, Line::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line() : BaseClass(),
_line(),
_points()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Line::Line ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F ) :
BaseClass( connection, tableName, id, srid, F ),
_line(),
_latLongPoints(),
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
//  Build the geometry.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry* Line::buildLineData()
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
    Magrathea::convertVerticesToEarthCoordinates( sampledPoints, *newVertices, this->spatialOffset().z() );

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
//  Convert to lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_convertToLatLong ( const Vertices& vertices, Vertices& latLongPoints )
{
  Usul::Interfaces::IProjectCoordinates::QueryPtr project ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IProjectCoordinates::IID ) );

  if ( project.valid() )
  {
    latLongPoints.reserve ( vertices.size() );
    for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      Usul::Math::Vec3d point;
      project->projectToSpherical( *iter, this->srid(), point );     
      latLongPoints.push_back( point );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lat long points.
//
///////////////////////////////////////////////////////////////////////////////

void Line::_buildLatLongPoints()
{
  _points.clear();

  const VertexList& vertexList ( this->_vertices() );

  for( VertexList::const_iterator iter = vertexList.begin(); iter != vertexList.end(); ++iter )
  {
    LatLongPoints latLongPoints;
    BaseClass::_convertToLatLong( *iter, latLongPoints );
    _points.push_back ( latLongPoints );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::lineData ()
{
  if( _latLongPoints.empty() )
  {
    this->_convertToLatLong ( _line, _latLongPoints );
  }

  return _latLongPoints;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Line::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::ILineData::IID:
    return static_cast < Usul::Interfaces::ILineData* > ( this );
  default:
    return BaseClass::queryInterface( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the line data.
//
///////////////////////////////////////////////////////////////////////////////

void Line::line( const Vertices& data )
{
  _line = data;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the line data.
//
///////////////////////////////////////////////////////////////////////////////

const Line::Vertices& Line::line() const
{
  return _line;
}
