
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Points/OctTree.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Strings/Format.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Make.h"
#include "Usul/File/Remove.h"
#include "Usul/Threads/ThreadId.h"

#include "osg/Geometry"
#include "osg/Geode"
#include "osg/Vec3"
#include "osg/Vec4"
#include "osg/BoundingBox"

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( OctTree, OctTree::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

OctTree::OctTree():
_tree( 0x0 ),
_capacity( 1000 ),
_buffer ( new StreamBuffer ( 4096 ) )
{
  // create a temp directory in the temp directory location
  const std::string tempDir ( Usul::File::Temp::directory ( false ) ); 
  const std::string dataDir ( Usul::Convert::Type< Usul::Types::Uint64, std::string >::convert ( Usul::Threads::currentThreadId() ) );
  const std::string path ( Usul::Strings::format ( tempDir, '/', dataDir, '/' ) );
  Usul::Functions::safeCallV1 ( &Usul::File::make, path, "3990628981" );

  // set the temp directory member variable so it can be deleted later
  _tempPath = path;
  std::cout << "Temp directory location is: " << _tempPath << std::endl;

  // create the root octree node
  _tree = new OctTreeNode ( _buffer, path );

  // set the point capacity of the octree nodes
  _tree->capacity( _capacity );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

OctTree::~OctTree()
{
  Usul::File::remove( _tempPath );
}

/////////////////
//  PUBLIC
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

bool OctTree::insert( Point p )
{
  Guard guard ( this->mutex() );
  return _tree->add( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::bounds( osg::BoundingBox bb )
{
  Guard guard ( this->mutex() );
  _tree->boundingBox( bb );
  _tree->distance( _tree->getBoundingRadius() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of points from the linked list
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::buildVectors()
{
  Guard guard ( this->mutex() );
  _tree->buildVectors();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene tree from all octree nodes
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OctTree::buildScene( Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  osg::ref_ptr< osg::Group > group ( new osg::Group );
  
  group->addChild( this->_buildTransparentPlane() );
  group->addChild( _tree->buildScene( caller, progress ) );

  return group.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the capacity level
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::capacity( unsigned int t )
{ 
  Guard guard ( this->mutex() );
  _capacity = t; 
  _tree->capacity( t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the capacity level
//
///////////////////////////////////////////////////////////////////////////////

unsigned int OctTree::capacity()
{ 
  Guard guard ( this->mutex() );
  return _capacity; 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the octree
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::split()
{
  Guard guard ( this );
  _tree->split();
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* OctTree::_buildTransparentPlane()
{
  Guard guard ( this->mutex() );

  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );

  osg::BoundingBox bb = _tree->boundingBox();

  osg::Vec3d minCorner ( bb.xMin(), bb.yMin(), bb.zMin() );
  osg::Vec3d maxCorner ( bb.xMax(), bb.yMax(), bb.zMin() );

  vertices->push_back( minCorner );
  vertices->push_back( osg::Vec3d ( maxCorner.x(), minCorner.y(), minCorner.z() ) );
  vertices->push_back( osg::Vec3d ( minCorner.x(), maxCorner.y(), minCorner.z() ) );

  vertices->push_back( osg::Vec3d ( minCorner.x(), maxCorner.y(), minCorner.z() ) );
  vertices->push_back( osg::Vec3d ( maxCorner.x(), minCorner.y(), minCorner.z() ) );
  vertices->push_back( maxCorner );
  
  // set the vertices
  geometry->setVertexArray( vertices.get() );
  geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES ) );

  //set the color array
  osg::ref_ptr< osg::Vec4Array > color ( new osg::Vec4Array );
  color->push_back( osg::Vec4 ( 0.0, 0.0, 0.0, 0.0 ) );
  geometry->setColorArray( color.get() );
  geometry->setColorBinding( osg::Geometry::BIND_OVERALL );

  geode->addDrawable( geometry.get() );

  // Set the node to be transparent
  OsgTools::State::StateSet::setAlpha ( geode.get(), 0.0f );

  
  return geode.release();
}

/////////////////
//  PRIVATE
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////


