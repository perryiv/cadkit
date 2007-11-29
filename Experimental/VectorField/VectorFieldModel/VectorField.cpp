
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "VectorField.h"

#include "OsgTools/State/StateSet.h"

#include "osg/ref_ptr"
#include "osg/Geometry"
#include "osg/Geode"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

VectorField::VectorField () :
_grid()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the grid.
//
///////////////////////////////////////////////////////////////////////////////

void VectorField::grid( const Grid& grid )
{
  _grid = grid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grid.
//
///////////////////////////////////////////////////////////////////////////////

VectorField::Grid& VectorField::grid()
{
  return _grid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the grid.
//
///////////////////////////////////////////////////////////////////////////////

const VectorField::Grid& VectorField::grid() const
{
  return _grid;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Advect a particle.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VectorField::advectParticle ( double x, double y ) const
{
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );

  Triangle::RefPtr triangle ( const_cast < Triangle* > ( _grid.triangleAtLocation ( x, y ) ) );

  osg::Vec3 position ( x, y, 0.0 );
  vertices->push_back ( position );

  const unsigned int maxSteps ( 1000 );
  unsigned int numSteps ( 0 );

  while ( triangle.valid() && numSteps < maxSteps )
  {
    ++numSteps;

    Triangle::Vector v ( triangle->vectorAtPoint ( position ) );
    position = position + ( osg::Vec3 ( v[0], v[1], 0.0 ) * 0.01 );

    if ( false == triangle->pointInside ( position ) )
    {
      triangle = const_cast < Triangle* > ( _grid.triangleAtLocation ( position[0], position[1] ) );
    }
    else
    {
      vertices->push_back ( position );
    }
  }

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  geode->addDrawable ( geometry.get() );

  geometry->setVertexArray ( vertices.get() );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( GL_LINE_STRIP, 0, vertices->size() ) );

  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array ( 1 ) );
  colors->at( 0 ).set ( 0.0, 0.0, 0.7, 1.0 );
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_OVERALL );

  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  OsgTools::State::StateSet::setLineWidth( geode.get(), 2.0 );

  return geode.release();
}
