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


namespace osg { class Material; class Geometry; }

class Cylinder
{
public:
  Cylinder(const osg::Vec3&, const osg::Vec3&);
  osg::Geometry * getGeometry(osg::Material *, unsigned int) const;
private:
  Cylinder();
  float _getDist(const osg::Vec3&, const osg::Vec3&) const;
  osg::Vec3 _point1, _point2;
};


#endif