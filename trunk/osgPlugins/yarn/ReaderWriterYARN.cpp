
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterYARN.h"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "Usul/Adaptors/Random.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::ReaderWriterYARN() :
  _meshes()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::~ReaderWriterYARN()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterYARN::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "yarn" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterYARN::className()
{
  return "YARN Reader";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::Result ReaderWriterYARN::readNode ( const std::string &file, const Options *options )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterYARN::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterYARN::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterYARN::Result ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterYARN::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Random number generator.
  Random<float> rd ( 0.0f, 1.0f );

  // Colors used below.
  osg::Vec4 emissive ( 0.0f, 0.0f, 0.0f, 1.0f );
  osg::Vec4 specular ( 0.8f, 0.8f, 0.8f, 1.0f );

  // Loop through the meshes.
  for ( Meshes::const_iterator i = _meshes.begin(); i != _meshes.end(); ++i )
  {
    // Generate the node.
    osg::ref_ptr< osg::Node > node ( (*i)() );

    // Make a random colors.
    osg::Vec4 diffuse ( rd(), rd(), rd(), 1.0f );
    osg::Vec4 ambient ( diffuse );

    // Make a material.
    osg::ref_ptr<osg::Material> mat ( new osg::Material() );
	  mat->setAmbient   ( osg::Material::FRONT_AND_BACK, ambient  );
	  mat->setDiffuse   ( osg::Material::FRONT_AND_BACK, diffuse  );
	  mat->setEmission  ( osg::Material::FRONT_AND_BACK, emissive );
    mat->setSpecular  ( osg::Material::FRONT_AND_BACK, specular );
	  mat->setShininess ( osg::Material::FRONT_AND_BACK, 100      );

    // Set the material.
    osg::ref_ptr< osg::StateSet > ss ( node->getOrCreateStateSet() );
    ss->setAttribute( mat.get() );

    // Add new mesh to the root.
    root->addChild ( node.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize internal data structures.
//
///////////////////////////////////////////////////////////////////////////////

void  ReaderWriterYARN::_init()
{
  _meshes.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::Result ReaderWriterYARN::_read ( const std::string &filename, const Options *options )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2346450991: Failed to open file: " + filename );

  // Parse the file.
  this->_parseYarns ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the yarns.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseYarns ( std::ifstream& fin )
{
  const unsigned int size ( 1024 );
  char buf[size];

  unsigned int numYarns ( 0 );
  unsigned int numSlices ( 0 );

  while ( !fin.eof() )
  {
    // Read the line.
    fin.getline ( buf, size );

    // Get the first word.
    std::istringstream in ( buf );
    std::string word;
    in >> word;

    // Convert to lower-case.
    std::transform ( word.begin(), word.end(), word.begin(), ::tolower );

    if ( word == "total_yarn_number" )
    {
      in >> numYarns;
      _meshes.resize ( numYarns );
    }

    else if ( word == "number_of_slices" )
    {
      in >> numSlices;
    }

    else if ( word == "yarn_number" )
    {
      this->_parseNumStacks ( fin, numYarns, numSlices );
    }

    else if ( word == "center_point" )
    {
      osg::Vec3 center;
      in >> center[0] >> center[1] >> center[2];
      this->_parseStack ( fin, numSlices, center );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the number of stacks.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseNumStacks ( std::ifstream& fin, unsigned int numYarns, unsigned int numSlices )
{
  unsigned int yarn, stacks;
  ++numSlices; // We close the loop.

  for ( unsigned int i = 0; i < numYarns; ++i )
  {
    fin >> yarn >> stacks;
    _meshes.at(yarn).points().resize  ( stacks * numSlices );
    _meshes.at(yarn).normals().resize ( stacks * numSlices );
    _meshes.at(yarn).size ( numSlices, stacks );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the stack.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseStack ( std::ifstream& fin, unsigned int numSlices, const osg::Vec3 &center )
{
  unsigned int yarn, stack, slice;
  float x, y, z;

  for ( unsigned int i = 0; i < numSlices; ++i )
  {
    // Read the data.
    fin >> yarn >> stack >> slice >> x >> y >> z;

    // Set the vertex.
    Vertex &v = _meshes.at(yarn).point ( slice, stack );
    v[0] = x;
    v[1] = y;
    v[2] = z;

    // Set the normal.
    Normal &n = _meshes.at(yarn).normal ( slice, stack );
    n = v - center;
    n.normalize();
  }

  // Close the loop by setting the last vertex to be the first.
  _meshes.at(yarn).point  ( numSlices, stack ) = _meshes.at(yarn).point  ( 0, stack );
  _meshes.at(yarn).normal ( numSlices, stack ) = _meshes.at(yarn).normal ( 0, stack );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterYARN> g_ReaderWriter_YARN_proxy;
