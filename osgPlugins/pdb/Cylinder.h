///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_CYLINDER_H__
#define __OSG_PDB_PLUGIN_CYLINDER_H__

#include "osg/Vec3"
#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/Referenced"

namespace osg { class Material; class Geometry; }

class Cylinder : public osg::Referenced
{
public:
  Cylinder();
  Cylinder(unsigned int sides);
  //osg::Geometry * getGeometry(osg::Material *, unsigned int) const;
  osg::Geometry * getGeometry() { return _geometry.get(); }
  bool valid() { return _point1.valid() && _point2.valid(); }
private:
  osg::Vec3 _point1, _point2, _center;
  osg::ref_ptr< osg::Geometry > _geometry;
};


#endif