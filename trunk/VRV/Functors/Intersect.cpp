
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
#include "Usul/Interfaces/IWorldInfo.h"
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
  _hasHit    ( false ),
  _drawRay   ( false )
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
  _hasHit    ( cb._hasHit ),
  _drawRay   ( cb._drawRay )
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

bool Intersect::_intersect ( osg::Node *scene, osgUtil::LineSegmentIntersector::Intersection& hit, const osg::Vec3d& start, const osg::Vec3d& end )
{
  // Handle bad input.
  if ( 0x0 == scene )
    return false;

  // Make the intersector.
  osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector ( new osgUtil::LineSegmentIntersector ( start, end ) );

  // Declare the pick-visitor.
  typedef osgUtil::IntersectionVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );
  visitor->setIntersector ( intersector.get() );

  // Calculate the intersections.
  scene->accept ( *visitor );

  // Get the hit-list for our line-segment.
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
  const Intersections &hits = intersector->getIntersections();
  if ( hits.empty() )
    return false;

  // Set the hit.
  hit = intersector->getFirstIntersection();

  return true;
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

  Usul::Interfaces::IWorldInfo::QueryPtr wi ( this->caller() );

  double zFar ( wi.valid() ? wi->worldRadius() * 2 : this->_farClippingDistance() );
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

double Intersect::_farClippingDistance() const
{
#if 1
  double nearDist, farDist;
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
  this->clearScene();

  // Query for needed interfaces.
  VRV::Interfaces::INavigationScene::QueryPtr ns ( this->caller() );

  // Return if we don't have the needed interface.
  if ( false == ns.valid() )
    return;

  // Determine the start and end of the ray.
  osg::Vec3 start, end;
  this->_rayBounds ( start, end );

  // Draw the ray if we should.
  if ( this->isDrawRay() )
  {
    // Make the ray.
    OsgTools::Ray ray;
    ray.thickness ( 2.0f );
    Usul::Math::Vec4f c ( 1.0, 0.0, 0.0, 1.0 );
    ray.color ( osg::Vec4 ( c[0], c[1], c[2], c[3] ) );
    ray.start ( start );
    ray.end ( end );
    
    _rayBranch->addChild ( ray() );
  }

  // Reset our state.
  _hasHit = false;

  // Intersect.
  osgUtil::LineSegmentIntersector::Intersection hit;
  if ( this->_intersect ( ns->navigationScene(), hit, start, end ) )
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
    geode->addDrawable ( OsgTools::ShapeFactory::instance().sphere( 5.0 ) );
    autoTransform->addChild ( geode.get() );
    osg::ref_ptr <osg::Material>  mat ( new osg::Material );
    osg::Vec4 color ( 1.0, 0.0, 0.0, 1.0 );
    mat->setDiffuse ( osg::Material::FRONT_AND_BACK, color );
    mat->setAmbient ( osg::Material::FRONT_AND_BACK, color );
    OsgTools::State::StateSet::setMaterial ( geode.get(), mat.get() );

    osg::ref_ptr < osg::StateSet > ss ( geode->getOrCreateStateSet() );

    ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );
    ss->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED );

    // Set the render bin.
    ss->setRenderBinDetails( 100, "RenderBin" );

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

Intersect::Intersection Intersect::lastHit() const
{
  return _hit;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear scene.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::clearScene()
{
  // Remove what we have.
  OsgTools::Group::removeAllChildren ( _rayBranch.get() );
  OsgTools::Group::removeAllOccurances ( _rayBranch.get(), this->_auxiliaryScene() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the draw ray flag.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::drawRay( bool b )
{
  Guard guard ( this );
  _drawRay = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the draw ray flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Intersect::isDrawRay() const
{
  Guard guard ( this );
  return _drawRay;
}
