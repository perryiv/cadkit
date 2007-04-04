
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/postGIS/Polygon.h"

#include "Usul/Interfaces/ITriangulate.h"
#include "Usul/Interfaces/IComputeTriangleNormals.h"
#include "Usul/Components/Manager.h"

#include "Magrathea/Project.h"
#include "Magrathea/Subdivide.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"

using namespace Minerva::Core::postGIS;

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Polygon, Polygon::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::Polygon ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F ) :
BaseClass( connection, tableName, id, srid, F ),
_points()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Polygon::~Polygon()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lat long height points.
//
///////////////////////////////////////////////////////////////////////////////

void Polygon::_buildLatLongPoints()
{
  _points.clear();

  // Typedefs
  typedef Usul::Interfaces::ITriangulate              Triangulate;
  typedef Triangulate::Vertices                       Vertices;
  typedef Triangulate::Vertices3D                     Vertices3D;

  // Find the needed plugins
  Triangulate::QueryPtr triangulate ( Usul::Components::Manager::instance().getInterface( Triangulate::IID ) );

  if( false == triangulate.valid() )
    return;

  VertexList vertexList ( _parser->getVertices() );

  for( VertexList::iterator iter = vertexList.begin(); iter != vertexList.end(); ++iter )
  {
    Vertices     vertices;
    Vertices3D   n;

    // Create the triangles
    triangulate->triangulate ( *iter, vertices, n );

    std::vector< ossimGpt > latLongPoints;
    this->_convertToLatLong( vertices, latLongPoints );

    std::vector< ossimGpt > subdividedPoints;
    Magrathea::subdivide( latLongPoints, subdividedPoints );

    _points.push_back ( subdividedPoints );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get polygon node.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Polygon::buildPolygonData()
{
  // Typedefs.
  typedef Usul::Interfaces::IComputeTriangleNormals   ComputeNormals;
  typedef ComputeNormals::Vertices                    Vertices;
  typedef ComputeNormals::Normals                     Normals;

  // Find the needed plugin.
  ComputeNormals::QueryPtr computeNormals ( Usul::Components::Manager::instance().getInterface ( ComputeNormals::IID ) );

  osg::ref_ptr< osg::Group > group ( new osg::Group );

  if( false == computeNormals.valid() )
    return group.release();

  if( _points.empty() )
    this->_buildLatLongPoints();

  for( PointsList::iterator iter = _points.begin(); iter != _points.end(); ++iter )
  {
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    osg::ref_ptr < osg::Geometry > geom ( new osg::Geometry );

    Vertices earthVertices;
    Normals  normals;

    // Convert vertices to earth coordinates.
    Magrathea::convertVerticesToEarthCoordinates( *iter, earthVertices, this->spatialOffset().z() );

    // Calculate the normals.
    computeNormals->computeNormalsPerTriangle( earthVertices, normals );

    /// Copy the osg vertices
    osg::ref_ptr< osg::Vec3Array > triangleVertices( new osg::Vec3Array );
    triangleVertices->reserve( earthVertices.size() );
    for( Normals::iterator iter = earthVertices.begin(); iter != earthVertices.end(); ++iter )
      triangleVertices->push_back( osg::Vec3 ( (*iter)[0], (*iter)[1], (*iter)[2] ) );


    /// Copy the osg normals.
    osg::ref_ptr < osg::Vec3Array > triangleNormals ( new osg::Vec3Array() );
    triangleNormals->reserve( normals.size() );
    for( Normals::iterator iter = normals.begin(); iter != normals.end(); ++iter )
      triangleNormals->push_back( osg::Vec3 ( (*iter)[0], (*iter)[1], (*iter)[2] ) );

    geom->setVertexArray ( triangleVertices.get() );
    geom->setNormalArray ( triangleNormals.get() );
    geom->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

    //geom->setUseDisplayList( true );

    geom->addPrimitiveSet ( new osg::DrawArrays ( GL_TRIANGLES, 0, triangleVertices->size() ) );

    geode->addDrawable( geom.get() );

    group->addChild ( geode.get() );
  }

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Polygon::queryInterface( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IPolygonData::IID:
    return static_cast < Usul::Interfaces::IPolygonData* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}
