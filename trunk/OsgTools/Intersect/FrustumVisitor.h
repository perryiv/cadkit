  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_FRUSTUM_VISITOR_H__
#define __OSG_TOOLS_FRUSTUM_VISITOR_H__

#include "OsgTools/Export.h"

#include "osg/NodeVisitor"
#include "osg/Polytope"
#include "osg/Geode"
#include "osg/Geometry"

#include "Frustum.h"

namespace OsgTools {
namespace Intersect {

class OSG_TOOLS_EXPORT Hit
{
public:
  Hit();
  Hit(const Hit& hit);
  ~Hit();
  
  Hit& operator = (const Hit& hit);
  
  typedef std::vector<int> VecIndexList;

  bool operator < (const Hit& hit) const
  {
    if (_originalFrustum<hit._originalFrustum) return true;
    if (_originalFrustum>hit._originalFrustum) return false;
    return _ratio<hit._ratio;
  }

  const osg::Vec3& getLocalIntersectPoint() const { return _intersectPoint; }
  const osg::Vec3& getLocalIntersectNormal() const { return _intersectNormal; }
  
  const osg::Vec3 getWorldIntersectPoint() const { if (_matrix.valid()) return _intersectPoint*(*_matrix); else return _intersectPoint; }
  const osg::Vec3 getWorldIntersectNormal() const ;

  float                           _ratio;
  osg::ref_ptr<Frustum>           _originalFrustum;
  osg::ref_ptr<Frustum>           _localFrustum;
  osg::NodePath                   _nodePath;
  osg::ref_ptr<osg::Geode>        _geode;
  osg::ref_ptr<osg::Drawable>     _drawable;
  osg::ref_ptr<osg::RefMatrix>    _matrix;
  osg::ref_ptr<osg::RefMatrix>    _inverse;
  
  VecIndexList                    _vecIndexList;
  int                             _primitiveIndex;
  osg::Vec3                       _intersectPoint;
  osg::Vec3                       _intersectNormal;

};

/// TODO.  Handle clipping planes.  If a vertex is clipped, it shouldn't be hit.
class OSG_TOOLS_EXPORT FrustumVisitor : public osg::NodeVisitor
{
public:
  typedef osg::NodeVisitor BaseClass;
  typedef std::vector<Hit> HitList;
  typedef std::map<const Frustum*,HitList > HitListMap;

  FrustumVisitor ( );
  virtual ~FrustumVisitor();

  void reset();

  void addFrustum ( Frustum *f );

  HitList& getHitList( const Frustum* f ) { return _hitList[f]; }

  virtual void apply(osg::Node&);
  virtual void apply(osg::Geode& node);

  virtual void apply(osg::Group& node);
  virtual void apply(osg::Transform& node);
  virtual void apply(osg::Switch& node);
  virtual void apply(osg::LOD& node);

protected:

  class IntersectState : public osg::Referenced
  {
  public:
    IntersectState();

    osg::ref_ptr<osg::RefMatrix> _matrix;
    osg::ref_ptr<osg::RefMatrix> _inverse;

    typedef std::pair< osg::ref_ptr< Frustum >, osg::ref_ptr< Frustum > >   FrustumPair;
    typedef std::vector< FrustumPair >                                      FrustumList;
    FrustumList _frustumList;

    typedef unsigned int FrustumMask;
    typedef std::vector<FrustumMask> FrustumMaskStack;
    FrustumMaskStack _frustumMaskStack;

    bool isCulled(const osg::BoundingSphere& bs, FrustumMask& segMaskOut);
    bool isCulled(const osg::BoundingBox& bb,    FrustumMask& segMaskOut);

    void addFrustumPair( Frustum* first, Frustum* second)
    {
      _frustumList.push_back(FrustumPair(first,second));
    }

  protected:
    ~IntersectState();
  };

  bool _intersect( osg::Drawable& drawable );

  void _pushMatrix(const osg::Matrix& matrix);
  void _popMatrix();

  bool _enterNode( osg::Node& node );
  void _leaveNode();

  typedef std::vector<osg::ref_ptr<IntersectState> > IntersectStateStack;
  IntersectStateStack         _intersectStateStack;

  HitListMap _hitList;
};

}
}

#endif // __OSG_TOOLS_FRUSTUM_VISITOR_H__

