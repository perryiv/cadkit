
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

#include "OsgTools/Sphere.h"
#include "OsgTools/Box.h"

#include <sstream>
#include <iostream>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::ReaderWriterCRSS() :
  _min ( 0, 0, 0 ),
  _max ( 1, 1, 1 ),
  _spheres(),
  _cubes()
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
//  Template function to help create geometry.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class ShapeType, class Sequence > struct Builder
  {
    static osg::Group *build ( const Sequence &shapes, const osg::Vec4 &activeColor, const osg::Vec4 &periodicColor )
    {
      // Make a group.
      osg::ref_ptr<osg::Group> group ( new osg::Group );

      // Loop through the sequence.
      for ( Sequence::const_iterator i = shapes.begin(); i != shapes.end(); ++i )
      {
        // Shortcuts.
        const osg::Vec3 &center = i->first;
        float size ( i->second.first );

        // Make the shape. Not so many polygons.
        ShapeType shape ( size, 0.25f );

        // Give the shapes materials.
        shape.color_policy().ambient ( ( i->second.second ) ? activeColor : periodicColor );
        shape.color_policy().diffuse ( shape.color_policy().ambient() );

        // Move it to the center.
        osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
        mt->setMatrix ( osg::Matrixf::translate ( center ) );

        // Add sphere to matrix-transform.
        mt->addChild ( shape() );

        // Add matrix-transform to group.
        group->addChild ( mt.get() );
      }

      // Return the group.
      return group.release();
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterCRSS::_build() const
{
  typedef Detail::Builder<OsgTools::MaterialBox,Cubes> CubeBuilder;
  typedef Detail::Builder<OsgTools::MaterialSphere,Spheres> SphereBuilder;

  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Make the shapes.
  osg::ref_ptr<osg::Group> cubes   ( CubeBuilder::build   ( _cubes,   osg::Vec4 ( 1, 0, 0, 1 ), osg::Vec4 ( 0.5, 0, 0, 1 ) ) );
  osg::ref_ptr<osg::Group> spheres ( SphereBuilder::build ( _spheres, osg::Vec4 ( 0, 1, 0, 1 ), osg::Vec4 ( 0, 0.5, 0, 1 ) ) );

  // Add the cubes and spheres.
  root->addChild ( cubes.get()   );
  root->addChild ( spheres.get() );
#if 1
  // Add the wire-frame boundary.
  OsgTools::MaterialBox cube ( _max[0] - _min[0], _max[1] - _min[1], _max[2] - _min[2] );
  osg::ref_ptr<osg::Node> boundary ( cube() );
  osg::ref_ptr<osg::StateSet> ss ( boundary->getOrCreateStateSet() );
  osg::ref_ptr<osg::PolygonMode> pm ( new osg::PolygonMode );
  pm->setMode ( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
  ss->setAttributeAndModes ( pm.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

  // Add the boundary.
  root->addChild ( boundary.get() );
#endif
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterCRSS::Result ReaderWriterCRSS::_read ( const std::string &filename, const Options *options )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

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
