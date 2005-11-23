
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Callbacks/BoundingSphere.h"

using namespace OsgTools::Callbacks;

BoundingSphere::BoundingSphere ( const osg::BoundingSphere &bs ) :
_bs ( bs )
{
}

osg::BoundingSphere BoundingSphere::computeBound(const osg::Node&) const
{
  return _bs;
}
