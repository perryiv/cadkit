///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#ifndef __OSG_PDB_PLUGIN_CYLINDER_H__
#define __OSG_PDB_PLUGIN_CYLINDER_H__

#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/Vec3"
#include "osg/LineWidth"

namespace osg { class Material; }

class Cylinder
{
public:
  Cylinder();
  Cylinder(const osg::Vec3&, const osg::Vec3&);
  osg::Geometry * getGeometry(osg::Material *, float) const;
private:
  osg::Vec3 _point1, _point2;
};


#endif