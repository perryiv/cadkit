
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

#include "Precompiled.h"
#include "Intersect.h"

#include "CadViewer/ErrorChecker.h"
#include "CadViewer/Constants.h"
#include "CadViewer/Functors/Direction.h"

#include "Usul/Math/Vector4.h"

#include "osgUtil/IntersectVisitor"

#include "osg/Group"

#include "OsgTools/Ray.h"
#include "OsgTools/Group.h"

using namespace CV;
using namespace CV::Pick;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::Intersect ( 
  Unknown *unknown, 
  Direction *dir,
  unsigned int id ) : 
  BaseClass ( unknown, id ),
  _direction ( dir ),
  _auxScene  ( unknown ),
  _navScene  ( unknown ),
  _models    ( unknown ),
  _wandState ( unknown ),
  _clipDist  ( unknown ),
  _rayBranch ( new osg::Group ),
  _visitor   ( new osgUtil::IntersectVisitor ),
  _segment   ( new osg::LineSegment ),
  _nodePath  ( new Intersect::Path ),
  _point     ( 0, 0, 0 ),
  _normal    ( 1, 0, 0 )
{
  ErrorChecker ( 1069043160, 0x0 != _nodePath.get() );
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
  _navScene  ( cb._navScene ),
  _models    ( cb._models ),
  _wandState ( cb._wandState ),
  _clipDist  ( cb._clipDist ),
  _rayBranch ( cb._rayBranch ),
  _visitor   ( cb._visitor ),
  _segment   ( cb._segment ),
  _nodePath  ( new Intersect::Path ( *(cb._nodePath) ) ),
  _point     ( cb._point ),
  _normal    ( cb._normal )
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

void Intersect::_intersect ( osg::Node *scene )
{
  ErrorChecker ( 1069049315, 0x0 != _nodePath.get() );

  // Initialize the path.
  _nodePath->clear();

  // Handle bad input.
  if ( 0x0 == scene )
    return;

  // Determine the start and end of the ray.
  osg::Vec3 start, end;
  this->_rayBounds ( start, end );

  // Set the line segment's bounds.
  _segment->set ( start, end );

  // Reset the intersect-visitor and add the line segment to it.
  _visitor->reset();
  _visitor->addLineSegment ( _segment.get() );

  // Calculate the intersections.
  scene->accept ( *_visitor );

  // Save the info if we intersected.
  osgUtil::IntersectVisitor::HitList &hits = _visitor->getHitList ( _segment.get() );
  if ( hits.size() )
  {
    (*_nodePath) = hits[0]._nodePath;
    _point  = hits[0].getWorldIntersectPoint();
    _normal = hits[0].getWorldIntersectNormal();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Do we have a valid intersection?
//
///////////////////////////////////////////////////////////////////////////////

bool Intersect::_valid() const
{
  ErrorChecker ( 1071206559, 0x0 != _nodePath.get() );
  return ( false == _nodePath->empty() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last intersection.
//
///////////////////////////////////////////////////////////////////////////////

const Intersect::Path& Intersect::_path() const
{
  ErrorChecker ( 1069041787, 0x0 != _nodePath.get() );
  return *_nodePath;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the last intersection.
//
///////////////////////////////////////////////////////////////////////////////

Intersect::Path& Intersect::_path()
{
  ErrorChecker ( 1069050673, 0x0 != _nodePath.get() );
  return *_nodePath;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the ray in the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::_updateRayScene()
{
  // Determine the start and end of the ray.
  osg::Vec3 start, end;
  this->_rayBounds ( start, end );

  // Make the ray.
  OsgTools::Ray ray;
  ray.thickness ( DEFAULT_RAY_THICKNESS );
  Usul::Math::Vec4f c ( DEFAULT_RAY_COLOR );
  ray.color ( osg::Vec4 ( c[0], c[1], c[2], c[3] ) );
  ray.start ( start );
  ray.end ( end );

  // Add a "ray" to the scene.
  OsgTools::Group::removeAllChildren ( _rayBranch.get() );
  _rayBranch->addChild ( ray() );
  OsgTools::Group::removeAllOccurances ( _rayBranch.get(), this->_auxiliaryScene() );
  this->_auxiliaryScene()->addChild ( _rayBranch.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the ray-branch from the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Intersect::reset()
{
  OsgTools::Group::removeAllChildren ( _rayBranch.get() );
  OsgTools::Group::removeAllOccurances ( _rayBranch.get(), this->_auxiliaryScene() );
  _nodePath->clear();
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
  end = start + dir * zFar;
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
//  Return the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Intersect::_navigationScene() const
{
  return _navScene->navigationScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the navigation scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Intersect::_navigationScene()
{
  return _navScene->navigationScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the model scene.
//
///////////////////////////////////////////////////////////////////////////////

const osg::Group *Intersect::_modelsScene() const
{
  return _models->modelsScene();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the model scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *Intersect::_modelsScene()
{
  return _models->modelsScene();
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
  float nearDist, farDist;
  _clipDist->getClippingDistances ( nearDist, farDist );
  return farDist;
}
