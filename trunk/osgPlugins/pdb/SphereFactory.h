
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
  typedef osg::ref_ptr<osg::Geometry> Sphere;
  typedef std::vector<Sphere> Spheres;

  SphereFactory();

  // Create a sphere.
  osg::Geometry*        create ( unsigned int numDivisions );

protected:

  virtual ~SphereFactory();

private:

  Spheres _spheres;
};


#endif // __OSG_PDB_PLUGIN_SPHERE_FACTORY_H__
