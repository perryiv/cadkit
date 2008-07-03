
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Points/PointSet.h"
#include "OsgTools/Configure/OSG.h"

#include "osgDB/WriteFile"

#include "osg/BoundingBox"

#include <iostream>

//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( PointSet, PointSet::BaseClass );

using namespace OsgTools::Points;
USUL_IMPLEMENT_TYPE_ID ( PointSet );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PointSet::PointSet() : BaseClass (),
_points( new osg::Vec3Array ),
_tree( new OctTree() )
{
  // Remove this after testing
#if 0
  osg::BoundingBox bb ( 1352000.0f, -70900.0f, 4300.0f, 1355000.0f, -70000.0f, 4900.0f );
  _tree->bounds( bb );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

PointSet::~PointSet()
{
 
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::clear ( Unknown *caller )
{
  Guard guard ( this->mutex() );
  _points->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *PointSet::buildScene ( Unknown *caller )
{
  Guard guard ( this->mutex() );
  GroupPtr group ( new osg::Group );
  group->addChild( _tree->buildScene( caller ) );
  return group.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::preBuildScene( Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this->mutex() );
  _tree->preBuildScene( document, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of points from the linked list
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::buildVectors()
{
  Guard guard ( this->mutex() );
  _tree->buildVectors();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

bool PointSet::addPoint( float x, float y, float z )
{
  Guard guard ( this );
  osg::Vec3d p ( static_cast< double > ( x ), static_cast< double > ( y ), static_cast< double > ( z ) );
  return this->addPoint( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

bool PointSet::addPoint( double x, double y, double z )
{
  Guard guard ( this );
  return this->addPoint( osg::Vec3d ( x, y, z ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

bool PointSet::addPoint( osg::Vec3 p )
{
  Guard guard ( this );
  osg::Vec3d point ( static_cast< double > ( p.x() ), static_cast< double > ( p.y() ), static_cast< double > ( p.z() ) );
  
#if 1 // With spatial partitioning
  return _tree->insert( point );
#else // NO spatial partitioning
  _points->push_back( point );
#endif

  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Allocate space for this point set
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::allocate( unsigned int num )
{
  Guard guard ( this );
  _points->reserve( num );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounds of the data
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::bounds( osg::Vec3f min, osg::Vec3f max )
{
  Guard guard ( this );
  osg::BoundingBox bb ( min.x(), min.y(), min.z(), max.x(), max.y(), max.z() );
  _tree->bounds( bb );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the capacity level for the octree
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::capacity( unsigned int t )
{
  Guard guard ( this );
  _tree->capacity( t );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create the octree
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::split( Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this );
  _tree->split( document, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the binary restart file for this point set
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::write( std::ofstream* ofs, Usul::Types::Uint64 numPoints, Usul::Documents::Document* document, Unknown *caller, Unknown *progress ) const
{
  Guard guard ( this );

  // Check the size of a float value to ensure it is 4 bytes
  if( 4 != sizeof( float ) )
  {
     throw std::runtime_error ( "Error 1250760804: Size of float is not 4 bytes. " ); 
  }

  // Check to see if the file was opened
  if( false == ofs->is_open() )
    throw std::runtime_error ( "Error 2024821453: Failed to open file: " );

  // Set progress message
  document->setStatusBar( "Writing Binary Restart File...", progress );

  // Write binary header
  const std::string id ( "CDBEF2DA-CCD4-4c30-844C-22A988DFE37C" ); 
  ofs->write ( &id[0], id.size() );

  // write the number of points
  ofs->write ( reinterpret_cast< char* > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );

  _tree->write( ofs, document, caller, progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the binary restart file
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::read( std::ifstream* ifs, Usul::Types::Uint64 &numPoints, Usul::Documents::Document* document, Unknown *caller, Unknown *progress )
{
  Guard guard ( this );

  // Check the size of a float value to ensure it is 4 bytes
  if( 4 != sizeof( float ) )
  {
     throw std::runtime_error ( "Error 4995251770: Size of float is not 4 bytes. " ); 
  }

  // Set progress message
  document->setStatusBar( "Reading Binary Restart File...", progress );

  // Read binary header
  const std::string id ( "CDBEF2DA-CCD4-4c30-844C-22A988DFE37C" );
  std::vector<char> header ( id.size() );
  ifs->read ( &header[0], header.size() );
  const bool same ( id == std::string ( header.begin(), header.end() ) );
  if( false == same )
  {
    throw std::runtime_error ( "Error 2348749452: Invalid binary file format! " );
  }

  // read the number of points
  ifs->read ( reinterpret_cast< char* > ( &numPoints ), sizeof( Usul::Types::Uint64 ) );

  _tree->read( ifs, document, caller, progress );
}

