
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ShapeFactory.h"

#include "Usul/Algorithms/Sphere.h"

#include <vector>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ShapeFactory::ShapeFactory() : BaseClass(),
  _latLongSpheres(),
  _cubes()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ShapeFactory::~ShapeFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the internal maps.
//
///////////////////////////////////////////////////////////////////////////////

void ShapeFactory::clear()
{
  _latLongSpheres.clear();
  _cubes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a cube. If one was already created with these inputs, then that 
//  same cube is returned.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry *ShapeFactory::cube ( const osg::Vec3 &size )
{
  // See if we have this one already...
  Cubes::iterator iter = _cubes.find ( size );
  if ( _cubes.end() != iter )
    return iter->second.get();

  // Otherwise, make the new sphere...
  // Half lengths
  const float hw ( 0.5f * size[0] );
  const float hh ( 0.5f * size[1] );
  const float hd ( 0.5f * size[2] );

  // Vertices and normals.
  osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array );

  // Front face
  vertices->push_back ( osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( osg::Vec3 ( -hw,  hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 0, 1 ) );

  // Back face
  vertices->push_back ( osg::Vec3 (  hw, -hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( osg::Vec3 (  hw,  hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 0, -1 ) );

  // Top face
  vertices->push_back ( osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw,  hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( 0, 1, 0 ) );

  // Bottom face
  vertices->push_back ( osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( osg::Vec3 ( -hw, -hh,  hd ) );
  vertices->push_back ( osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( osg::Vec3 (  hw, -hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 0, -1, 0 ) );

  // Left face
  vertices->push_back ( osg::Vec3 ( -hw,  hh,  hd ) );
  vertices->push_back ( osg::Vec3 ( -hw,  hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw, -hh, -hd ) );
  vertices->push_back ( osg::Vec3 ( -hw, -hh,  hd ) );
  normals->push_back  ( osg::Vec3 ( -1, 0, 0 ) );

  // Right face
  vertices->push_back ( osg::Vec3 (  hw,  hh, -hd ) );
  vertices->push_back ( osg::Vec3 (  hw,  hh,  hd ) );
  vertices->push_back ( osg::Vec3 (  hw, -hh,  hd ) );
  vertices->push_back ( osg::Vec3 (  hw, -hh, -hd ) );
  normals->push_back  ( osg::Vec3 ( 1, 0, 0 ) );

  // Make a new geometry and add the vertices and normals.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  // Save this geometry in our collection and return it.
  _cubes[size] = geometry;
  return geometry.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a sphere meshed in the latitude-longitude way. If one was 
//  already created with these inputs, then that same sphere is returned.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry *ShapeFactory::sphere ( float radius, 
                                      const MeshSize &size,
                                      const LatitudeRange &latRange,
                                      const LongitudeRange &longRange )
{
  // See if we have this one already...
  SphereProperties key ( SphereSize ( radius, size ), SphereRange ( latRange, longRange ) );
  LatLongSpheres::iterator iter = _latLongSpheres.find ( key );
  if ( _latLongSpheres.end() != iter )
    return iter->second.get();

  // Otherwise, make the new sphere...
  // Define the generator.
  typedef osg::Vec3Array Points;
  typedef osg::Vec3Array Normals;
  typedef std::pair < unsigned int, unsigned int > Strip;
  typedef std::vector < Strip > Strips;
  typedef Usul::Algorithms::LatLongSphere < float > Sphere;

  // The points, normals, and indices.
  osg::ref_ptr<Points>  points  ( new Points  );
  osg::ref_ptr<Normals> normals ( new Normals );
  Strips strips;

  // Declare the generator and set some properties.
  Sphere sphere;
  sphere.radius         ( radius );
  sphere.normalLength   ( 1.0f );
  sphere.latitudeStart  ( Usul::Math::DEG_TO_RAD * latRange.first );
  sphere.latitudeEnd    ( Usul::Math::DEG_TO_RAD * latRange.second );
  sphere.longitudeStart ( Usul::Math::DEG_TO_RAD * longRange.first );
  sphere.longitudeEnd   ( Usul::Math::DEG_TO_RAD * longRange.second );
  sphere.numLatitude    ( size.first );
  sphere.numLongitude   ( size.second );

  // Generate the data.
  sphere ( *points, *normals, strips );

  // Make a new geometry.
  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( points.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  // The primitives.
  typedef osg::Geometry::PrimitiveSetList Prims;
  unsigned int numStrips ( strips.size() );
  Prims prims ( numStrips );

  // Loop through and set all the primitives.
  for ( unsigned int i = 0; i < numStrips; ++i )
  {
    unsigned int start ( strips.at(i).first );
    unsigned int count ( strips.at(i).second );
    prims.at(i) = new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, start, count );
  }

  // Set the geometry's primitive list.
  geometry->setPrimitiveSetList ( prims );

  // Save this geometry in our collection and return it.
  _latLongSpheres[key] = geometry;
  return geometry.get();
}
