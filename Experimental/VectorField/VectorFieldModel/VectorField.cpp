
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "VectorField.h"

#include "OsgTools/State/StateSet.h"
#include "OsgTools/ShapeFactory.h"

#include "osg/ref_ptr"
#include "osg/Geometry"
#include "osg/Geode"
#include "osg/Material"
#include "osg/MatrixTransform"


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

  const unsigned int maxSteps ( 5000 );
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

namespace Detail
{
  osg::Node* drawCriticalPoint ( const Triangle::Position& p, Triangle::CriticalPointType type )
  {
    osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
    unsigned int size ( static_cast < unsigned int > ( 15.0 ) );
    OsgTools::ShapeFactory::MeshSize meshSize ( size, size );
    OsgTools::ShapeFactory::LatitudeRange  latRange  ( 89.9f, -89.9f );
    OsgTools::ShapeFactory::LongitudeRange longRange (  0.0f, 360.0f );

    geode->addDrawable( OsgTools::ShapeFactory::instance().sphere ( 0.040, meshSize, latRange, longRange  ) );

    osg::ref_ptr < osg::Material > mat ( new osg::Material );

    osg::Vec4 color ( 0.5, 0.5, 0.5, 1.0 );

    if ( Triangle::SADDLE_POINT == type )
      color.set ( 0.0, 1.0, 0.0, 1.0 );
    else if ( Triangle::ATTRACTING_NODE == type )
      color.set ( 1.0, 0.0, 1.0, 1.0 );
    else if ( Triangle::REPELLING_NODE == type )
      color.set ( 1.0, 1.0, 0.0, 1.0 );
    else if ( Triangle::CENTER == type )
      color.set ( 0.6, 0.6, 1.0, 1.0 );
    else if ( Triangle::ATTRACTING_FOCUS == type )
      color.set ( 1.0, 0.7, 0.7, 1.0 );
    else if ( Triangle::REPELLING_FOCUS == type )
      color.set ( 1.0, 0.5, 0.0, 1.0 );

    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );

    geode->getOrCreateStateSet()->setAttributeAndModes ( mat.get(), osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
    osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
    mt->setMatrix ( osg::Matrix::translate ( p ) );
    mt->addChild ( geode.get() );

    return mt.release();
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Classify.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* VectorField::buildTopographyGraph () const
{
  osg::ref_ptr < osg::Group > group ( new osg::Group );

  for ( unsigned int i = 0; i < _grid.rows(); ++i )
  {
    for ( unsigned int j = 0; j < _grid.rows(); ++j )
    {
      Cell2D::RefPtr cell ( const_cast < Cell2D* > ( _grid.cell ( i, j ) ) );

      if ( cell.valid() )
      {
        Triangle::RefPtr t0 ( cell->upper() );
        Triangle::RefPtr t1 ( cell->lower() );

        {
          Triangle::Position p ( t0->findCriticalPoint() );
          if ( t0->pointInside ( p ) )
          {
            Triangle::CriticalPointType type ( t0->classifyCriticalPoint ( p ) );
            group->addChild ( Detail::drawCriticalPoint ( p, type ) );
          }
        }

        {
          Triangle::Position p ( t1->findCriticalPoint() );
          if ( t1->pointInside ( p ) )
          {
            Triangle::CriticalPointType type ( t1->classifyCriticalPoint ( p ) );
            group->addChild ( Detail::drawCriticalPoint ( p, type ) );
          }
        }
      }
    }
  }

  return group.release();
}
