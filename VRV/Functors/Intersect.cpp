
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class for functors that transform the scene.
//
///////////////////////////////////////////////////////////////////////////////

#include "VRV/Functors/Intersect.h"
#include "VRV/Interfaces/INavigationScene.h"

#include "Usul/Functors/Interaction/Navigate/Direction.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Math/Vector4.h"

#include "osg/AutoTransform"
#include "osg/Group"
#include "osg/Geode"
#include "osg/Material"
#include "osg/LineWidth"

#include "OsgTools/Ray.h"
#include "OsgTools/Builders/Arrow.h"
#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/ShapeFactory.h"
#include "OsgTools/MaterialSetter.h"

#include <iostream>
#include <iomanip>

using namespace VRV;
using namespace VRV::Functors;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::Intersect ( 
  Unknown *unknown, 
  Direction *dir,
  const std::string& name ) : 
  BaseClass ( unknown, name ),
  _direction ( dir ),
  _auxScene  ( unknown ),
  _wandState ( unknown ),
  _clipDist  ( unknown ),
  _rayBranch ( new osg::Group ),
  _hit       (),
  _hasHit    ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::Intersect ( const Intersect &cb ) : 
  BaseClass   ( cb ),
  _direction ( cb._direction ),
  _auxScene  ( cb._auxScene ),
  _wandState ( cb._wandState ),
  _clipDist  ( cb._clipDist ),
  _rayBranch ( cb._rayBranch ),
  _hit       ( cb._hit ),
  _hasHit    ( cb._hasHit )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::~Intersect()
{
  // Make sure our node is removed from the application's scene.
  OsgTools::Group::removeAllOccurances ( _rayBranch.get(), this->_auxiliaryScene() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect a ray with the scene.
//
///////////////////////////////////////////////////////////////////////////////

bool Intersect::_intersect ( osg::Node *scene, osgUtil::Hit& hit )
{
  // Handle bad input.
  if ( 0x0 == scene )
    return false;

  // Determine the start and end of the ray.
  osg::Vec3 start, end;
  this->_rayBounds ( start, end );

  // Make the line segment.
  osg::ref_ptr < osg::LineSegment > segment ( new osg::LineSegment );

  // Set the line segment's bounds.
  segment->set ( start, end );

  // Make the visitor.
  osg::ref_ptr < osgUtil::IntersectVisitor > visitor ( new osgUtil::IntersectVisitor );

  visitor->addLineSegment ( segment.get() );

  // Calculate the intersections.
  scene->accept ( *visitor );

  // Save the info if we intersected.
  osgUtil::IntersectVisitor::HitList &hits = visitor->getHitList ( segment.get() );
  if ( hits.size() > 0 )
  {
    hit = hits.front();
    return true;
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Determine the start and end of the ray.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::_rayBounds ( osg::Vec3& start, osg::Vec3& end )
{
  this->_wandPosition ( start );
  
  (*_direction)();
  const Direction::Vector &d = _direction->vector();
  osg::Vec3 dir ( d[0], d[1], d[2] );

  float zFar ( this->_farClippingDistance() );
  end = start + ( dir * zFar );

#if 0
  Usul::Interfaces::ITextMatrix::QueryPtr tm ( this->caller () );
  if ( tm.valid() )
  {
    const unsigned int size ( 1024 );
    std::vector < char > startString ( size, '\0' );
    std::vector < char > endString ( size, '\0' );
    std::vector < char > chars ( size, '\0' );

    ::sprintf ( &startString[0], "Start: %12.6f %12.6f %12.6f\n", start[0], start[1], start[2] ); 
    ::sprintf ( &endString[0],     "End: %12.6f %12.6f %12.6f\n", end[0], end[1], end[2] ); 
    ::sprintf ( &chars[0],   "Direction: %12.6f %12.6f %12.6f\n", dir[0], dir[1], dir[2] ); 

    tm->setText ( 15, 75, std::string ( &chars[0] ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    tm->setText ( 15, 45, std::string ( &endString[0] ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    tm->setText ( 15, 15, std::string ( &startString[0] ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
  }
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the auxiliary scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Intersect::_auxiliaryScene() const
{
  return _auxScene->auxiliaryScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the auxiliary scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Intersect::_auxiliaryScene()
{
  return _auxScene->auxiliaryScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand's position.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::_wandPosition ( osg::Vec3 &p ) const
{
  WandState::Vector v;
  _wandState->wandPosition ( v );
  p.set ( v[0], v[1], v[2] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the far clipping distance.
//
///////////////////////////////////////////////////////////////////////////////

float Intersect::_farClippingDistance() const
{
#if 1
  float nearDist, farDist;
  _clipDist->getClippingDistances ( nearDist, farDist );
  return farDist;
#else
  return 30;
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::operator()()
{
  // Remove what we have.
  OsgTools::Group::removeAllChildren ( _rayBranch.get() );
  OsgTools::Group::removeAllOccurances ( _rayBranch.get(), this->_auxiliaryScene() );

  // Query for needed interfaces.
  VRV::Interfaces::INavigationScene::QueryPtr ns ( this->caller() );

  // Return if we don't have the needed interface.
  if ( false == ns.valid() )
    return;
  
#if 0
  // Determine the start and end of the ray.
  osg::Vec3 start, end;
  this->_rayBounds ( start, end );

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  osg::ref_ptr<osg::Geometry> geom ( new osg::Geometry );
  osg::ref_ptr<osg::Vec3Array> p ( new osg::Vec3Array );

  p->resize ( 2 );
  (*p)[0] = start;
  (*p)[1] = end;

  geom->setVertexArray ( p.get() );
  geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINES, 0, 2 ) );

  osg::ref_ptr<osg::Vec4Array> c ( new osg::Vec4Array );
  c->resize ( 2 );
  (*c)[0] = osg::Vec4 ( 1.0, 0.0, 0.0, 1.0 );
  (*c)[1] = osg::Vec4 ( 0.0, 1.0, 0.0, 1.0 );

  geom->setColorArray ( c.get() );
  geom->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );

  osg::ref_ptr<osg::LineWidth> lw ( new osg::LineWidth );
  lw->setWidth ( 2 );

  osg::ref_ptr<osg::StateSet> state = geom->getOrCreateStateSet();
  state->setAttribute ( lw.get() );
  state->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );

  geode->addDrawable ( geom.get() );

#if 0
  // Make the ray.
  OsgTools::Ray ray;
  ray.thickness ( 2.0f );
  Usul::Math::Vec4f c ( 1.0, 0.0, 0.0, 1.0 );
  ray.color ( osg::Vec4 ( c[0], c[1], c[2], c[3] ) );
  ray.start ( start );
  ray.end ( end );
#endif

  _rayBranch->addChild ( geode.get() );
#endif

  // Reset our state.
  _hasHit = false;

  osgUtil::Hit hit;
  if ( this->_intersect ( ns->navigationScene(), hit ) )
  {
    // Set our internal members.
    _hasHit = true;
    _hit = hit;

    // Center of the sphere.
    osg::Vec3 center ( hit.getWorldIntersectPoint() );

#if 0
    Usul::Interfaces::ITextMatrix::QueryPtr tm ( this->caller () );
    if ( tm.valid() )
    {
      const unsigned int size ( 1024 );
      std::vector < char > chars ( size, '\0' );

      ::sprintf ( &chars[0],   "Intersection Point: %12.6f %12.6f %12.6f\n", center[0], center[1], center[2] ); 

      tm->setText ( 15, 15, std::string ( &chars[0] ), osg::Vec4 ( 1.0, 1.0, 1.0, 1.0 ) );
    }
#endif

    osg::ref_ptr< osg::AutoTransform > autoTransform ( new osg::AutoTransform );

    autoTransform->setPosition ( center );
    autoTransform->setAutoScaleToScreen ( true );

    // Set the normalize state to true, so when the sphere size changes it still looks correct.
    OsgTools::State::StateSet::setNormalize ( autoTransform.get(), true );
    OsgTools::State::StateSet::setTwoSidedLighting ( autoTransform.get(), true );

    osg::ref_ptr <osg::Geode> geode ( new osg::Geode );
    geode->addDrawable ( OsgTools::ShapeFactorySingleton::instance().sphere( 5.0 ) );
    autoTransform->addChild ( geode.get() );
    osg::ref_ptr <osg::Material>  mat ( new osg::Material );
    osg::Vec4 color ( 1.0, 0.0, 0.0, 1.0 );
    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
    mat->setAmbient ( osg::Material::FRONT_AND_BACK, color );
    OsgTools::State::StateSet::setMaterial ( geode.get(), mat.get() );
    geode->getOrCreateStateSet()->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    _rayBranch->addChild ( autoTransform.get() );
  }

  this->_auxiliaryScene()->addChild ( _rayBranch.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clone.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::BaseFunctor* Intersect::clone()
{
  return new Intersect ( *this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is there a hit?
//
///////////////////////////////////////////////////////////////////////////////

bool Intersect::hasHit() const
{
  return _hasHit;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last hit?
//
///////////////////////////////////////////////////////////////////////////////

osgUtil::Hit Intersect::lastHit() const
{
  return _hit;
}

