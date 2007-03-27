
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterCRSS.h"

#include "osgDB/ReadFile"
#include "osgDB/FileNameUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/MatrixTransform"
#include "osg/PolygonMode"
#include "osg/LOD"
#include "osg/Material"
#include "osg/LineWidth"

#include "OsgTools/Lod.h"
#include "OsgTools/State/StateSet.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <limits>


///////////////////////////////////////////////////////////////////////////////
//
//  Colors.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec4 SPHERE_COLOR_ACTIVE   ( 0.0f, 1.0f, 0.0f, 1.0f );
const osg::Vec4 SPHERE_COLOR_PERIODIC ( 0.0f, 0.5f, 0.0f, 1.0f );
const osg::Vec4 CUBE_COLOR_ACTIVE     ( 1.0f, 0.0f, 0.0f, 1.0f );
const osg::Vec4 CUBE_COLOR_PERIODIC   ( 0.5f, 0.0f, 0.0f, 1.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Lods.
//
///////////////////////////////////////////////////////////////////////////////

const float MAX_DISTANCE_FACTOR ( 100 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::ReaderWriterCRSS() : BaseClass(),
  _min          ( 0, 0, 0 ),
  _max          ( 1, 1, 1 ),
  _spheres      (),
  _cubes        (),
  _triangles    (),
  _lines        (),
  _factory      ( new OsgTools::ShapeFactory ),
  _numLodKids   ( 5 ),
  _lodDistPower ( 1 ),
  _segsLat      ( 3, 25 ),
  _segsLong     ( 6, 25 ),
  _colors       ()
{
  // Populate the color map.
  _colors.insert ( ColorMap::value_type ( "aqua",    osg::Vec4 ( 0,   220, 220, 1 ) ) );
  _colors.insert ( ColorMap::value_type ( "red",     osg::Vec4 ( 220, 0,   0,   1 ) ) );
  _colors.insert ( ColorMap::value_type ( "green",   osg::Vec4 ( 0,   220, 0,   1 ) ) );
  _colors.insert ( ColorMap::value_type ( "blue",    osg::Vec4 ( 0,   0,   220, 1 ) ) );
  _colors.insert ( ColorMap::value_type ( "purple",  osg::Vec4 ( 220, 0,   220, 1 ) ) );
  _colors.insert ( ColorMap::value_type ( "pink",    osg::Vec4 ( 220, 120, 190, 1 ) ) );
  _colors.insert ( ColorMap::value_type ( "orange",  osg::Vec4 ( 220, 120, 0,   1 ) ) );
  _colors.insert ( ColorMap::value_type ( "yellow",  osg::Vec4 ( 210, 207, 65,  1 ) ) );
  _colors.insert ( ColorMap::value_type ( "lime",    osg::Vec4 ( 148, 212, 6,   1 ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::~ReaderWriterCRSS()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterCRSS::acceptsExtension ( const std::string &ext ) const
{
  return osgDB::equalCaseInsensitive ( ext, "crss" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterCRSS::className()
{
  return "CRSS Reader/Writer";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::Result ReaderWriterCRSS::readNode ( const std::string &file, const Options *options ) const
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterCRSS::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterCRSS::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterCRSS::Result ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the material.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_material ( bool actual, osg::Material *am, osg::Material *pm, osg::Node *node ) const
{
  osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
  ss->setAttribute ( ( actual ) ? am : pm );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a cube.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_makeCube ( float size ) const
{
  // Make a cube.
  osg::ref_ptr<osg::Geometry> geometry ( _factory->cube ( osg::Vec3 ( size, size, size ) ) );

  // Add the geometry to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Return the geode holding all the cube.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make a sphere.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_makeSphere ( float radius, const osg::Vec2 &detail ) const
{
  // Determine the number of latitudinal and longitudinal segments.
  const unsigned int latitude  ( _segsLat[0]  + detail[0] * ( _segsLat[1]  - _segsLat[0]  ) );
  const unsigned int longitude ( _segsLong[0] + detail[1] * ( _segsLong[1] - _segsLong[0] ) );

  // Define sphere properties.
  const ShapeFactory::MeshSize size ( latitude, longitude );
  const ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f ); // Don't close the poles.
  const ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

  // Make a sphere.
  osg::ref_ptr<osg::Geometry> geometry ( _factory->sphere ( radius, size, latRange, longRange ) );

  // Add the geometry to a geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );

  // Return the geode holding all the spheres.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make all the spheres under the lod.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_makeSphere ( float radius ) const
{
  // Make a lod.
  osg::ref_ptr<osg::LOD> lod ( new osg::LOD );

  // Add several spheres.
  const float denominator ( _numLodKids - 1 );
  for ( unsigned int i = 0; i < _numLodKids; ++i )
  {
    const float loop ( i );
    const float detail ( ::pow ( 1.0f - loop / denominator, _lodDistPower ) );
    lod->addChild ( this->_makeSphere ( radius, osg::Vec2 ( detail, detail ) ) );
  }

  // Last child.
  //lod->addChild ( this->_makeCube ( radius * 1.5 ) );

  // Set the centers and ranges.
  float lastRangeMax ( std::numeric_limits<float>::max() );
  OsgTools::Lod::setCenterAndRanges ( MAX_DISTANCE_FACTOR, lastRangeMax, lod.get() );

  // Return the lod.
  return lod.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the spheres.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_buildSpheres() const
{
  // Make the materials.
  const osg::Material::Face face ( osg::Material::FRONT );
  osg::ref_ptr<osg::Material> activeMaterial   ( new osg::Material );
  osg::ref_ptr<osg::Material> periodicMaterial ( new osg::Material );
  activeMaterial->setAmbient   ( face, SPHERE_COLOR_ACTIVE );
  activeMaterial->setDiffuse   ( face, SPHERE_COLOR_ACTIVE );
  periodicMaterial->setAmbient ( face, SPHERE_COLOR_PERIODIC );
  periodicMaterial->setDiffuse ( face, SPHERE_COLOR_PERIODIC );

  // The group to hold all spheres.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Loop through the spheres.
  for ( Spheres::const_iterator i = _spheres.begin(); i != _spheres.end(); ++i )
  {
    const bool &actual = i->second.second;
    const float &radius = i->second.first;
    const osg::Vec3 &center = i->first;

    // Need a matrix-transform to place it at the center.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    group->addChild ( mt.get() );
    mt->setMatrix ( osg::Matrixf::translate ( center ) );

    // Add the sphere.
    mt->addChild ( this->_makeSphere ( radius ) );

    // Set the material.
    this->_material ( actual, activeMaterial.get(), periodicMaterial.get(), mt.get() );
  }

  // Return the group of spheres.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the cubes.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_buildCubes() const
{
  // Make the materials.
  const osg::Material::Face face ( osg::Material::FRONT );
  osg::ref_ptr<osg::Material> activeMaterial   ( new osg::Material );
  osg::ref_ptr<osg::Material> periodicMaterial ( new osg::Material );
  activeMaterial->setAmbient   ( face, CUBE_COLOR_ACTIVE );
  activeMaterial->setDiffuse   ( face, CUBE_COLOR_ACTIVE );
  periodicMaterial->setAmbient ( face, CUBE_COLOR_PERIODIC );
  periodicMaterial->setDiffuse ( face, CUBE_COLOR_PERIODIC );

  // The group to hold all cubes.
  osg::ref_ptr<osg::Group> group ( new osg::Group );

  // Loop through the cubes.
  for ( Cubes::const_iterator i = _cubes.begin(); i != _cubes.end(); ++i )
  {
    const bool &actual = i->second.second;
    const float &size = i->second.first;
    const osg::Vec3 &center = i->first;

    // Need a matrix-transform to place it at the center.
    osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
    group->addChild ( mt.get() );
    mt->setMatrix ( osg::Matrixf::translate ( center ) );

    // Add the cube.
    mt->addChild ( this->_makeCube ( size ) );

    // Set the material.
    this->_material ( actual, activeMaterial.get(), periodicMaterial.get(), mt.get() );
  }

  // Return the group of cubes.
  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangles.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* ReaderWriterCRSS::_buildTriangles() const
{
  // The geode to hold all triangles.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

  // Loop through the triangles.
  // This is horribly inefficent, but it gets the job done.
  for ( Triangles::const_iterator i = _triangles.begin(); i != _triangles.end(); ++i )
  {
    // Make the geometry.
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );

    // Make the material.
    osg::ref_ptr < osg::Material > material ( new osg::Material );

    // Get the color from the color map.
    ColorMap::const_iterator iter ( _colors.find ( i->color ) );
    osg::Vec4 color ( iter != _colors.end() ? iter->second : osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

    material->setAmbient ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.2, 0.2, 0.2, 1.0 ) );
    material->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
    material->setSpecular ( osg::Material::FRONT_AND_BACK, osg::Vec4 ( 0.5, 0.5, 0.5, 1.0 ) );
    material->setShininess ( osg::Material::FRONT_AND_BACK, 100 );

    // Make a polygon-mode.
    osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );

    osg::PolygonMode::Mode mode ( i->mode == "fill" ? osg::PolygonMode::FILL : osg::PolygonMode::LINE );

    // Set the polygon mode
    pm->setMode ( osg::PolygonMode::FRONT_AND_BACK, mode );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( material.get(), Attribute::OVERRIDE | Attribute::ON );
    ss->setAttributeAndModes ( pm.get(), Attribute::OVERRIDE | Attribute::ON );

    // Make the vertices.
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );

    // Add the vertices
    vertices->push_back ( i->v0 );
    vertices->push_back ( i->v1 );
    vertices->push_back ( i->v2 );

    // Calculate the normal.
    const osg::Vec3 t1 ( i->v1 - i->v0 );
    const osg::Vec3 t2 ( i->v2 - i->v0 );
    osg::Vec3 norm ( t1 ^ t2 );
    norm.normalize();

    // Make the normal array.
    osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array );
    normals->push_back ( norm );

    geometry->setVertexArray ( vertices.get() );
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_TRIANGLES, 0, 3 ) );

    geode->addDrawable ( geometry.get() );
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the lines.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node * ReaderWriterCRSS::_buildLines() const
{
  // The geode to hold all lines.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  OsgTools::State::StateSet::setTwoSidedLighting ( geode.get(), true );

  // Loop through the lines.
  // This is horribly inefficent, but it gets the job done.
  for ( Lines::const_iterator i = _lines.begin(); i != _lines.end(); ++i )
  {
    // Make the geometry.
    osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

    // Get the state set.
    osg::ref_ptr< osg::StateSet > ss ( geometry->getOrCreateStateSet() );

    // Get the color from the color map.
    ColorMap::const_iterator iter ( _colors.find ( i->color ) );
    osg::Vec4 color ( iter != _colors.end() ? iter->second : osg::Vec4 ( 0.0, 0.0, 0.0, 1.0 ) );

    osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
    lw->setWidth( i->thickness );

    // Set the state. Make it override any other similar states.
    typedef osg::StateAttribute Attribute;
    ss->setAttributeAndModes ( lw.get(), Attribute::OVERRIDE | Attribute::ON );

    // Make the vertices.
    osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );

    // Add the vertices
    vertices->push_back ( i->v0 );
    vertices->push_back ( i->v1 );

    // Make the color array.
    osg::ref_ptr< osg::Vec4Array > colors ( new osg::Vec4Array );
    colors->push_back ( color );

    geometry->setVertexArray ( vertices.get() );
    geometry->setColorArray ( colors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

    geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_LINES, 0, 2 ) );

    geode->addDrawable ( geometry.get() );
  }

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the boundary.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterCRSS::_buildBounds() const
{
  // Determine the size and center.
  const osg::Vec3 size   ( _max[0] - _min[0], _max[1] - _min[1], _max[2] - _min[2] );
  const osg::Vec3 hs     ( size * 0.5f );
  const osg::Vec3 center ( _min[0] + hs[0], _min[1] + hs[1], _min[2] + hs[2] );

  // Position the cube.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrixf::translate ( center ) );

  // Make the cube.
  osg::ref_ptr<osg::Geode> cube ( new osg::Geode );
  cube->addDrawable ( _factory->cube ( size ) );
  mt->addChild ( cube.get() );

  // Make it wire-frame.
  osg::ref_ptr<osg::StateSet> ss ( cube->getOrCreateStateSet() );
  osg::ref_ptr<osg::PolygonMode> mode ( new osg::PolygonMode );
  mode->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  ss->setAttributeAndModes ( mode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Return the cube.
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterCRSS::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Add the spheres.
  root->addChild ( this->_buildSpheres() );

  // Add the cubes.
  root->addChild ( this->_buildCubes() );

  // Add the triangles.
  root->addChild( this->_buildTriangles() );

  // Add the lines.
  root->addChild ( this->_buildLines() );

  // Add the boundary.
  root->addChild ( this->_buildBounds() );

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize internal data structures.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_init() const
{
  _spheres.clear();
  _cubes.clear();
  _triangles.clear();
  _lines.clear();

  // If we release the factory, then the user can close child windows and 
  // free up memory. If we do not release the factory, then multiple big 
  // files do not use a lot more memory than a single big file. Not yet sure 
  // which is best.
  //_factory->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::Result ReaderWriterCRSS::_read ( const std::string &filename, const Options *options ) const
{
  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Make sure the internal data members are initialized.
  this->_init();

  // Open the file.
  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2787095996: Failed to open file: " + filename );

  // Read size of box.
  in >> _min[0] >> _max[0];
  in >> _min[1] >> _max[1];
  in >> _min[2] >> _max[2];

  // Needed below.
  char shape;

  // Loop until the end of the file.
  while ( EOF != in.peek() )
  {
    // Read the shape type.
    in >> shape;

    // Save the sphere or cube.
    if ( 'C' == shape || 'c' == shape )
      this->_readCube ( in );
    else if ( 'S' == shape || 's' == shape )
      this->_readSphere ( in );
    else if ( 'T' == shape || 't' == shape )
      this->_readTriangle ( in );
    else if ( 'L' == shape || 'l' == shape )
      this->_readLineSegment ( in );
    else
    {
      std::ostringstream message;
      message << "Error 3546502860: Unknown shape '" << shape << " found in file: " << filename;
      throw std::runtime_error ( message.str() );
    }
  }

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a cube.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_readCube ( std::istream& in ) const
{
  // Needed below.
  osg::Vec3 center;
  float size;
  char type;

  // Read the line.
  in >> type;
  in >> center[0] >> center[1] >> center[2];
  in >> size;

  // Is it an actual precipitate?
  bool actual ( 'A' == type || 'a' == type );

  _cubes.push_back ( Cube ( center, SizeAndType ( size, actual ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a sphere.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_readSphere ( std::istream& in ) const
{
  // Needed below.
  osg::Vec3 center;
  float size;
  char type;

  // Read the line.
  in >> type;
  in >> center[0] >> center[1] >> center[2];
  in >> size;

  // Is it an actual precipitate?
  bool actual ( 'A' == type || 'a' == type );

  _spheres.push_back ( Sphere ( center, SizeAndType ( size, actual ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a cube.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_readTriangle ( std::istream& in ) const
{
  Triangle t;

  in >> t.v0[0] >> t.v0[1] >> t.v0[2];
  in >> t.v1[0] >> t.v1[1] >> t.v1[2];
  in >> t.v2[0] >> t.v2[1] >> t.v2[2];
  in >> t.mode >> t.color;

  _triangles.push_back ( t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read a cube.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterCRSS::_readLineSegment ( std::istream& in ) const
{
  Line l;

  in >> l.v0[0] >> l.v0[1] >> l.v0[2];
  in >> l.v1[0] >> l.v1[1] >> l.v1[2];
  in >> l.thickness >> l.color;

  _lines.push_back( l );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterCRSS> g_ReaderWriter_CRSS_proxy;
