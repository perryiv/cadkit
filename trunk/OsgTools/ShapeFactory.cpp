
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

ShapeFactory::ShapeFactory() :
  _latLongSpheres()
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
