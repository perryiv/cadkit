
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_STL_PLUGIN_FACET_H__
#define __OSG_STL_PLUGIN_FACET_H__

#include "osg/Vec3"

namespace osg { class Geometry; }

class Facet
{
public:
  Facet();
  void setNormal(const osg::Vec3 &n) { _normal = n; }
  void setVector(const osg::Vec3 &v);
  const osg::Vec3 getNormal() const { return _normal; }
  const osg::Vec3 getV1() const { return _v1; }
  const osg::Vec3 getV2() const { return _v2; }
  const osg::Vec3 getV3() const { return _v3; }
  osg::Geometry* getGeometry();
private:
  osg::Vec3 _normal, _v1, _v2, _v3;
  int _numVec;
};

#endif

