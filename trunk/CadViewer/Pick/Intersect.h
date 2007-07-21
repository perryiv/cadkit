
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

#include "CadViewer/Functors/Direction.h"
#include "CadViewer/Interfaces/IAuxiliaryScene.h"
#include "VRV/Interfaces/INavigationScene.h"
#include "VRV/Interfaces/IModelsScene.h"
#include "VRV/Interfaces/IClippingDistance.h"
#include "VRV/Interfaces/IWandState.h"

#include "osg/Vec3"

#include <memory>
#include <vector>

namespace osg { class LineSegment; class Group;class Node; };
namespace osg { typedef std::vector<Node*> NodePath; };
namespace osgUtil { class IntersectVisitor; };

namespace CV {
namespace Pick {


class Intersect : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Intersect );
  typedef VRV::Functors::BaseFunctor BaseClass;

  // Reset any states.
  virtual void          reset();

protected:

  typedef osg::NodePath Path;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef CV::Functors::Direction Direction;
  typedef CV::Interfaces::IAuxiliaryScene AuxiliaryScene;
  typedef VRV::Interfaces::INavigationScene NavigationScene;
  typedef VRV::Interfaces::IModelsScene ModelsScene;
  typedef VRV::Interfaces::IWandStateFloat WandState;
  typedef VRV::Interfaces::IClippingDistanceFloat ClippingDistance;

  Intersect ( Unknown *unknown, Direction *dir, unsigned int id );
  Intersect ( const Intersect &cb );
  virtual ~Intersect();

  const osg::Group *    _auxiliaryScene() const;
  osg::Group *          _auxiliaryScene();

  float                 _farClippingDistance() const;

  void                  _intersect ( osg::Node *scene );

  const osg::Group *    _modelsScene() const;
  osg::Group *          _modelsScene();

  const osg::Group *    _navigationScene() const;
  osg::Group *          _navigationScene();

  const Path &          _path() const;
  Path &                _path();

  void                  _rayBounds ( osg::Vec3& start, osg::Vec3& end );

  void                  _updateRayScene();

  bool                  _valid() const;

  void                  _wandPosition ( osg::Vec3 &p ) const;

  const osg::Vec3 &     _worldIntersectPoint()  const { return _point; }
  const osg::Vec3 &     _worldIntersectNormal() const { return _normal; }

private:

  Direction::ValidRefPtr _direction;
  AuxiliaryScene::ValidQueryPtr _auxScene;
  NavigationScene::ValidQueryPtr _navScene;
  ModelsScene::ValidQueryPtr _models;
  WandState::ValidQueryPtr _wandState;
  ClippingDistance::ValidQueryPtr _clipDist;
  USUL_VALID_REF_POINTER ( osg::Group ) _rayBranch;
  USUL_VALID_REF_POINTER ( osgUtil::IntersectVisitor ) _visitor;
  USUL_VALID_REF_POINTER ( osg::LineSegment ) _segment;
  std::auto_ptr<Path> _nodePath;
  osg::Vec3 _point;
  osg::Vec3 _normal;
};


}; // namespace Pick
}; // namespace CV


#endif // _CV_INTERSECT_FUNCTORS_H_
