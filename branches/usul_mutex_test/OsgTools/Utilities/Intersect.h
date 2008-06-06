
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_BOUNDING_BOX_INTERSECT_H__
#define __OSG_TOOLS_BOUNDING_BOX_INTERSECT_H__

#include "OsgTools/Export.h"
#include "OsgTools/Configure/OSG.h"

#include "osg/BoundingBox"
#include "osg/Array"
#include "osg/Plane"

namespace OsgTools
{
  namespace Utilities
  {
    ///  Get the vertices where the plane intersects with the bounding box.
    OSG_TOOLS_EXPORT void boundingBoxIntersect ( const osg::Plane& plane, const osg::BoundingBox& bb, osg::Vec3Array &vertices );

    ///  Get the vertex where line segment intersects the plane.
    /// See "Algebraic Form" at http://en.wikipedia.org/wiki/Line-plane_intersection and
    /// See "Solution 2" at http://local.wasp.uwa.edu.au/~pbourke/geometry/planeline/
    inline bool lineSegmentIntersect ( const osg::Plane& plane, const osg::Vec3& v0, const osg::Vec3& v1, osg::Vec3& point )
    {
      osg::Vec4 p ( plane.asVec4() );

      double denom ( ( p.x() * ( v0.x() - v1.x() ) ) + ( p.y() * ( v0.y() - v1.y() ) ) + ( p.z() * ( v0.z() - v1.z() ) ) );

      // Line and the plane are parallel.
      if ( denom == 0.0 )
        return false;

      double num ( p.w() + ( p.x() * v0.x() ) + ( p.y() * v0.y() ) + ( p.z() * v0.z() ) );

      double u ( num / denom );

      // Line intersects, but does so past the line segment end points.
      //if ( u < 0.0 || u > 1.0 )
      //  return false;

      point = v0 + ( ( v1 - v0 ) * u );

      return true;
    }
  }
}

#endif // __OSG_TOOLS_BOUNDING_BOX_INTERSECT_H__

