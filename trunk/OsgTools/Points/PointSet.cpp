
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#include "PointSet.h"

#include "osg/BoundingBox"

#include "osgDB/WriteFile"

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
#if 1
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

#if 1 // With spatial partitioning
  group->addChild( _tree->buildScene( caller ) );
#else // NO spatial partitioning
  GeometryPtr geometry ( new osg::Geometry ); 
  geometry->setVertexArray( _points.get() );
  geometry->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, _points->size() ) );

  GeodePtr geode ( new osg::Geode );
  geode->addDrawable( geometry.get() );
  
  group->addChild( geode.get() );
#endif
  std::string filename ( "C:/testfile.osg" );
  //filename = filename + Usul::File::base( name );
  std::cout << "Writing file " << filename << "..." << std::endl;
  osgDB::writeNodeFile( *group.get(), filename );

  return group.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::addPoint( float x, float y, float z )
{
  Guard guard ( this );
  osg::Vec3d p ( static_cast< double > ( x ), static_cast< double > ( y ), static_cast< double > ( z ) );
  this->addPoint( p );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::addPoint( double x, double y, double z )
{
  Guard guard ( this );
  this->addPoint( osg::Vec3d ( x, y, z ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a point
//
///////////////////////////////////////////////////////////////////////////////

void PointSet::addPoint( osg::Vec3 p )
{
  Guard guard ( this );
  osg::Vec3d point ( static_cast< double > ( p.x() ), static_cast< double > ( p.y() ), static_cast< double > ( p.z() ) );
  
#if 1 // With spatial partitioning
  _tree->insert( point );
#else // NO spatial partitioning
  _points->push_back( point );
#endif
}
