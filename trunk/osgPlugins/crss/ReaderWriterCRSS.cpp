
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

#include "OsgTools/Lod.h"

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
  _factory      ( new OsgTools::ShapeFactory ),
  _numLodKids   ( 5 ),
  _lodDistPower ( 1 ),
  _segsLat      ( 3, 25 ),
  _segsLong     ( 6, 25 )
{
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

bool ReaderWriterCRSS::acceptsExtension ( const std::string &ext )
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

ReaderWriterCRSS::Result ReaderWriterCRSS::readNode ( const std::string &file, const Options *options )
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

void ReaderWriterCRSS::_init()
{
  _spheres.clear();
  _cubes.clear();

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

ReaderWriterCRSS::Result ReaderWriterCRSS::_read ( const std::string &filename, const Options *options )
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
  osg::Vec3 center;
  float size;
  char shape, type;

  // Loop until the end of the file.
  while ( EOF != in.peek() )
  {
    // Read the line.
    in >> shape >> type;
    in >> center[0] >> center[1] >> center[2];
    in >> size;

    // Is it an actual precipitate?
    bool actual ( 'A' == type || 'a' == type );

    // Save the sphere or cube.
    if ( 'C' == shape || 'c' == shape )
      _cubes.push_back ( Cube ( center, SizeAndType ( size, actual ) ) );
    else if ( 'S' == shape || 's' == shape )
      _spheres.push_back ( Sphere ( center, SizeAndType ( size, actual ) ) );
    else
    {
      std::ostringstream message;
      message << "Error 3546502860: Unknown shape '" << type << " found in file: " << filename;
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
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterCRSS> g_ReaderWriter_CRSS_proxy;
