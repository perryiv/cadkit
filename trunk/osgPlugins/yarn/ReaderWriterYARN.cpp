
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
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <locale>


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

osg::Group * ReaderWriterYARN::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  Random < float > rd ( 0.0, 1.0 );

  for( Meshes::const_iterator i = _meshes.begin(); i != _meshes.end(); ++ i)
  {
    osg::ref_ptr< osg::Node > node ( (*i)() );

    float r ( rd() );
	  float g ( rd() );
	  float b ( rd() );

    osg::ref_ptr< osg::Material > mat ( new osg::Material() );
	  mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	  mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(r, g, b, 1.0));
	  mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1, 0.1, 0.1, 1.0));
	  mat->setShininess(osg::Material::FRONT_AND_BACK, 100);
	  mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8, 0.8, 0.8, 1.0));

    osg::ref_ptr< osg::StateSet > ss ( node->getOrCreateStateSet() );
    ss->setAttribute( mat.get() );

    root->addChild( node.get() );
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

  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
   throw std::runtime_error ( "Error 2346450991: Failed to open file: " + filename );
  this->_parse ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////
void ReaderWriterYARN::_parse( std::ifstream& fin )
{
  const unsigned int size ( 512 );
  char buf[ size ];

  unsigned int totalYarnNumber ( 0 );
  unsigned int totalSliceNumber ( 0 );

  std::vector< int > stackNumbers;

  while ( !fin.eof() )
  {
    fin.getline( buf, size );

    std::istringstream in ( buf );

    std::string word;

    in >> word;

    if( word == "Total_Yarn_Number" )
    {
      in >> totalYarnNumber;
      _meshes.resize( totalYarnNumber );
    }
    else if ( word == "Number_of_Slices" )
      in >> totalSliceNumber;
    else if ( word == "Yarn_Number" )
    {
      stackNumbers.resize( totalYarnNumber );
      for( unsigned int i = 0; i < totalYarnNumber; ++i )
      {
        int yarnNumber, numStacks;
        fin.getline( buf, size );
        std::istringstream in ( buf );
        in >> yarnNumber >> numStacks;
        stackNumbers.at( yarnNumber ) = numStacks;
        _meshes.at( yarnNumber ).points().resize( numStacks * totalSliceNumber );
        _meshes.at( yarnNumber ).size( totalSliceNumber, numStacks );
      }
    }
    else if ( word == "center_point" )
    {
      for( unsigned int i = 0; i < totalSliceNumber; ++i )
      {
        int yarn, stack, slice;
        float x, y, z;
        fin.getline( buf, size );
        std::istringstream in ( buf );
        in >> yarn >> stack >> slice >> x >> y >> z;
        Vertex v;
        v = _meshes.at( yarn ).point( slice, stack );
        v[0] = x;
        v[1] = y;
        v[2] = z;
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterYARN> g_ReaderWriter_YARN_proxy;
