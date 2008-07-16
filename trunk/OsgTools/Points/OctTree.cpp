
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

#include <fstream>

using namespace OsgTools::Points;
USUL_IMPLEMENT_TYPE_ID ( OctTree );

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
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::preBuildScene( Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  _tree->preBuildScene( document, caller, progress );
}


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

void OctTree::split(  Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this );
  Usul::Types::Uint64 numPoints ( _tree->getNumPoints() );
  document->setStatusBar( "Step 2/2: Building Spatial Parameters...", progress );
  unsigned int d ( static_cast< unsigned int > ( static_cast< double > ( numPoints ) / static_cast< double > ( _tree->capacity() ) ) );
  _tree->initSplitProgress( 0, d );
  _tree->split( document, caller, progress );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Read the binary restart file
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::read ( std::ifstream* ifs, Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );

   // Read and set the number of leaf nodes
  Usul::Types::Uint64 numLeafNodes ( 0 );
  ifs->read ( reinterpret_cast< char* > ( &numLeafNodes ), sizeof( Usul::Types::Uint64 ) );
  _tree->numLeafNodes( numLeafNodes );

  // Read the number of lods
  Usul::Types::Uint64 numLods ( 0 );
  ifs->read ( reinterpret_cast< char* > ( &numLods ), sizeof( Usul::Types::Uint64 ) );

  // Read and set the lods
  std::vector< Usul::Types::Uint16 > lodDefinitions ( numLods );
  ifs->read ( reinterpret_cast< char* > ( &lodDefinitions[0] ), sizeof( lodDefinitions ) );
  _tree->lodDefinitions( lodDefinitions );

  // Read the lod max distance
  Usul::Types::Float64 d ( 0 );
  ifs->read ( reinterpret_cast< char* > ( &d ), sizeof( Usul::Types::Float64 ) );
  _tree->distance( static_cast< double > ( d ) );

  // Initialize progress
  _tree->initSplitProgress( 0, numLeafNodes * 2 );
 
  _tree->read( ifs, document, caller, progress );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the binary restart file for this octree
//
///////////////////////////////////////////////////////////////////////////////

void OctTree::write( std::ofstream* ofs, Usul::Documents::Document* document, Unknown *caller, Unknown *progress ) const
{
  Guard guard ( this );
  
  // Write the number of leaf nodes
  Usul::Types::Uint64 numLeafNodes ( _tree->numLeafNodes() );
  ofs->write ( reinterpret_cast< char* > ( &numLeafNodes ), sizeof( Usul::Types::Uint64 ) );

  // Write the lod definitions
  std::vector< Usul::Types::Uint16 > lodDefinitions ( _tree->getLodDefinitions() );
  Usul::Types::Uint64 numLods ( lodDefinitions.size() );

  // Write the number of lods
  ofs->write ( reinterpret_cast< char* > ( &numLods ), sizeof( Usul::Types::Uint64 ) );

  // Write the lods
  ofs->write ( reinterpret_cast< char* > ( &lodDefinitions[0] ), sizeof( lodDefinitions ) );

  // Write the lod max distance
  Usul::Types::Float64 d ( _tree->getBoundingRadius() );
  ofs->write ( reinterpret_cast< char* > ( &d ), sizeof( Usul::Types::Float64 ) );

  _tree->write( ofs, 0, numLeafNodes, document, caller, progress );
}


/////////////////
//  PROTECTED
/////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Create a transparent plane to enable seeking
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
  vertices->push_back( maxCorner );
  vertices->push_back( osg::Vec3d ( minCorner.x(), maxCorner.y(), minCorner.z() ) );

  // set the vertices
  geometry->setVertexArray( vertices.get() );
  geometry->addPrimitiveSet( new osg::DrawArrays ( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

  // Set the normals
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );
  normals->push_back( osg::Vec3 ( 0.0, 0.0, 1.0 ) );
  geometry->setNormalArray( normals.get() );
  geometry->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE );

  //set the color array
  osg::Vec4 color( osg::Vec4 ( 1.0, 1.0, 1.0, 0.0 ) );

  // add the geometry to the geode
  geode->addDrawable( geometry.get() );

  // Set the material properties
  OsgTools::State::StateSet::setMaterial( geode.get(), color, color, 0.0f );

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


