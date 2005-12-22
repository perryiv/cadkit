
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
  _cubes(),
  _cylinders()
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


///////////////////////////////////////////////////////////////////////////////
//
//  Create a cylinder. If one was already created with these inputs, then that 
//  same cylinder is returned.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry * ShapeFactory::cylinder ( float radius, 
                                         unsigned int sides, 
                                         const osg::Vec3& pointOne,
                                         const osg::Vec3& pointTwo )
{
  CylinderProperties key ( CylinderSize( radius, sides ), CylinderPoints( pointOne, pointTwo ) );
  Cylinders::iterator iter = _cylinders.find ( key );
  if ( _cylinders.end() != iter )
    return iter->second.get();

  Geometry geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr< osg::Vec3Array > normals  ( new osg::Vec3Array );

  //build a unit cylinder
  //osg::Vec3 unPoint1 (0,0,0);
  //osg::Vec3 unPoint2 (0,1,0);

  //_point1 = unPoint1;
  //_point2 = unPoint2;

  osg::Vec3 center;
  center[0] = (pointOne[0] + pointTwo[0])/2.0;
  center[1] = (pointOne[1] + pointTwo[1])/2.0;
  center[2] = (pointOne[2] + pointTwo[2])/2.0;

  float c = 3.14159 / 180.0;
  float x,y,z;
  //calculate the point for the unit cylinder then multiply it by the transform matrix
  for(unsigned int i = 0; i < sides; ++i)
  {
    float u = (float) i / (float) (sides - 1);
    float theta = u * 360.0;
    x = radius * sin( c * theta ) + pointOne[0];
    y = pointOne[1];
    z = radius * cos( c * theta ) + pointOne[2];
    vertices->push_back( osg::Vec3(x, y, z) );

    x = radius * sin( c * theta ) + pointTwo[0];
    y = pointTwo[1];
    z = radius * cos( c * theta ) + pointTwo[2];
    vertices->push_back( osg::Vec3(x, y, z) );
  }

  geometry->setVertexArray ( vertices.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, 0, vertices->size() ) );

  normals->reserve(vertices->size());

  //calculate the normals for the tri-strips
  for(osg::Vec3Array::iterator i = vertices->begin(); i != vertices->end(); ++ i)
  {
    osg::Vec3 normal ( *i );
    normal.normalize();
    normals->push_back(normal);
  }

  geometry->setNormalArray(normals.get());
  geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

  _cylinders[key] = geometry;
  return geometry.get();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a cone.  This will not cache the results.
//
///////////////////////////////////////////////////////////////////////////////

osg::Geometry * ShapeFactory::cone ( const osg::Vec3& center, 
                                     float radius, 
                                     float height, 
                                     const osg::Quat& rotation, 
                                     float tessellation )
{
  const unsigned int minNumSegments ( 5 );
  const unsigned int minNumRows ( 5 );

  unsigned int numSegments ( 40 );
  unsigned int numRows     ( 10 );

  if ( tessellation > 0.0f && tessellation != 1.0f) 
  {
    numRows = static_cast < unsigned int > ( numRows * tessellation );
    if ( numRows < minNumRows )
      numRows = minNumRows;
    numSegments = static_cast < unsigned int > ( numSegments * tessellation );
    if ( numSegments < minNumSegments )
      numSegments = minNumSegments;
  }

  float normalz ( radius / ( sqrtf( radius * radius + height * height ) ) );
  float normalRatio ( 1.0f / ( sqrtf( 1.0f + normalz * normalz ) ) );
  normalz *= normalRatio;

  float angleDelta = 2.0f*osg::PI/(float)numSegments;
  float texCoordHorzDelta = 1.0/(float)numSegments;
  float texCoordRowDelta = 1.0/(float)numRows;
  float hDelta = height / (float)numRows;
  float rDelta = radius / (float)numRows;

  float topz= height + ( -0.25f * height );
  float topr=0.0f;
  float topv=1.0f;
  float basez=topz-hDelta;
  float baser=rDelta;
  float basev=topv-texCoordRowDelta;
  float angle;
  float texCoord;

  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  osg::ref_ptr < osg::Vec3Array > normals  ( new osg::Vec3Array );

  for(unsigned int rowi=0; rowi<numRows;
    ++rowi,topz=basez, basez-=hDelta, topr=baser, baser+=rDelta, topv=basev, basev-=texCoordRowDelta) 
  {
    // we can't use a fan for the cone top
    // since we need different normals at the top
    // for each face..
    //glBegin(GL_QUAD_STRIP);

    angle = 0.0f;
    texCoord = 0.0f;
    for(unsigned int topi=0; topi<numSegments;
        ++topi,angle+=angleDelta,texCoord+=texCoordHorzDelta) 
    {
      float c = cosf(angle);
      float s = sinf(angle);

      osg::Vec3 normal ( c*normalRatio, s*normalRatio, normalz );
      normal = rotation * normal;
      normal += center;
      normals->push_back ( normal );
      normals->push_back ( normal );

      osg::Vec3 v1 ( c*topr, s*topr, topz );
      v1 = rotation * v1;
      v1 += center;

      vertices->push_back ( v1 );

      osg::Vec3 v2 ( c*baser, s*baser, basez );
      v2 = rotation * v2;
      v2 += center;

      vertices->push_back ( v2 );
    }

    // do last point by hand to ensure no round off errors.
    osg::Vec3 normal ( normalRatio, 0.0f, normalz );
    normal = rotation * normal;
    normal += center;
    normals->push_back ( normal );
    normals->push_back ( normal );

    osg::Vec3 v1 ( topr, 0.0f, topz );
    v1 = rotation * v1;
    v1 += center;
    vertices->push_back ( v1 );

    osg::Vec3 v2 ( baser, 0.0f, basez );
    v2 = rotation * v2;
    v2 += center;
    vertices->push_back ( v2 );

    //glEnd();
  }

  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );

  const unsigned int quadEnd ( vertices->size() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::QUAD_STRIP, 0, quadEnd ) );

  return geometry.release();
}
