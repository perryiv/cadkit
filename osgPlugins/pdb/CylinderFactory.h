
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_PDB_PLUGIN_CYLINDER_FACTORY_H__
#define __OSG_PDB_PLUGIN_CYLINDER_FACTORY_H__

#include "osg/ref_ptr"
#include "osg/Referenced"

#include "Cylinder.h"

#include <vector>

class CylinderFactory : public osg::Referenced
{
public:
  typedef osg::ref_ptr< CylinderFactory > Ptr;
  typedef std::vector< osg::ref_ptr<Cylinder> > Cylinders;
  CylinderFactory();

  osg::Geometry*  create(float r, unsigned int sides);
private:
  Cylinders _cylinders;
};

#endif
