
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class for functors that intersect the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERSECT_FUNCTORS_H_
#define _CV_INTERSECT_FUNCTORS_H_

#include "Usul/Functors/Interaction/Navigate/Direction.h"

#include "VRV/Interfaces/IAuxiliaryScene.h"

#include "Usul/Interfaces/IClippingDistance.h"
#include "Usul/Interfaces/IWandState.h"

#include "osg/Vec3"

#include "osgUtil/LineSegmentIntersector"

#include <memory>
#include <vector>

namespace osg { class Group; class Node; };

namespace VRV {
namespace Functors {


class Intersect : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Intersect );
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseFunctor;
  typedef BaseFunctor                                      BaseClass;
  typedef Usul::Functors::Interaction::Navigate::Direction Direction;
  typedef osgUtil::LineSegmentIntersector::Intersection    Intersection;

  Intersect ( Unknown *unknown, Direction *dir, const std::string& name );

  // Intersect.  Call this before hasHit or lastHit.
  virtual void          operator()();

  // Is there a hit?
  bool                  hasHit() const;

  // Get the last hit?
  Intersection          lastHit() const;

  // Clone this functor.
  BaseFunctor*          clone();

  // Clear scene.
  void                  clearScene();

  // Get/Set the draw ray flag.
  void                  drawRay( bool b );
  bool                  isDrawRay() const;
protected:

  typedef osg::NodePath Path;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef VRV::Interfaces::IAuxiliaryScene AuxiliaryScene;
  typedef Usul::Interfaces::IWandStateDouble WandState;
  typedef Usul::Interfaces::IClippingDistance ClippingDistance;

  Intersect ( const Intersect &cb );
  virtual ~Intersect();

  const osg::Group *    _auxiliaryScene() const;
  osg::Group *          _auxiliaryScene();

  double                _farClippingDistance() const;

  bool                  _intersect ( osg::Node *scene, osgUtil::LineSegmentIntersector::Intersection & hit, const osg::Vec3d& start, const osg::Vec3d& end );

  void                  _rayBounds ( osg::Vec3& start, osg::Vec3& end );

  void                  _wandPosition ( osg::Vec3 &p ) const;

private:

  Direction::ValidRefPtr _direction;
  AuxiliaryScene::ValidQueryPtr _auxScene;
  WandState::ValidQueryPtr _wandState;
  ClippingDistance::ValidQueryPtr _clipDist;
  USUL_VALID_REF_POINTER ( osg::Group ) _rayBranch;
  osgUtil::LineSegmentIntersector::Intersection _hit;
  bool _hasHit;
  bool _drawRay;
};


}; // namespace Pick
}; // namespace CV


#endif // _CV_INTERSECT_FUNCTORS_H_
