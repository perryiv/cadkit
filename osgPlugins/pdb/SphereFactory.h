
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_SPHERE_FACTORY_H__
#define __OSG_PDB_PLUGIN_SPHERE_FACTORY_H__

#include "osg/Referenced"
#include "osg/Vec3"
#include "osg/ref_ptr"

#include <vector>

namespace osg { class Geometry; };


class SphereFactory : public osg::Referenced
{
public:

  typedef osg::ref_ptr<SphereFactory> Ptr;
  typedef std::vector<osg::Vec3> Vertices;
  typedef std::vector<Vertices *> Spheres;

  SphereFactory();

  // Create a sphere.
  osg::Geometry*        create ( unsigned int numDivisions, const osg::Vec3 &center, float radius );

protected:

  virtual ~SphereFactory();

private:

  Spheres _spheres;
};


#endif // __OSG_PDB_PLUGIN_SPHERE_FACTORY_H__
