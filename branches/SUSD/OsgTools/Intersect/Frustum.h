  
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_INTERSECT_FRUSTUM_H__
#define __OSGTOOLS_INTERSECT_FRUSTUM_H__

#include "osg/Referenced"
#include "osg/Polytope"

namespace OsgTools {
namespace Intersect {

class OSG_TOOLS_EXPORT Frustum : public osg::Referenced
{
public:
  typedef osg::Polytope::PlaneList    PlaneList;
  typedef osg::Polytope::VertexList   VertexList;
  typedef osg::Polytope::ClippingMask ClippingMask;
  
  Frustum () : osg::Referenced(), _frustum() {}
      
  inline void add(const osg::Plane& pl)
  {
    _frustum.add ( pl );
  }

  inline PlaneList& getPlaneList()
  {
    return _frustum.getPlaneList();
  }

  inline const PlaneList& getPlaneList() const
  {
    return _frustum.getPlaneList();
  }

  inline bool contains(const osg::Vec3& v) const
  {
    return _frustum.contains( v );
  }

  inline bool contains(const std::vector< osg::Vec3 >& vertices)
  {
    return _frustum.contains( vertices );
  }

  inline bool contains(const osg::BoundingSphere& bs)
  {
    return _frustum.contains ( bs );
  }
  
  inline bool contains(const osg::BoundingBox& bb)
  {
    return _frustum.contains( bb );
  }

  inline bool containsAllOf(const std::vector< osg::Vec3>& vertices)
  {
    return _frustum.containsAllOf( vertices );
  }

  inline bool containsAllOf(const osg::BoundingSphere& bs)
  {
    return _frustum.containsAllOf ( bs );
  }
  
  inline bool containsAllOf(const osg::BoundingBox& bb)
  {
    return _frustum.containsAllOf( bb );
  }

  inline void transform(const osg::Matrix& matrix)
  {
    _frustum.transform ( matrix );
  }
  
  inline void transformProvidingInverse(const osg::Matrix& matrix)
  {
    _frustum.transformProvidingInverse( matrix );
  }

  // Find the intersection points between the line segment and the frustum.  See http://astronomy.swin.edu.au/~pbourke/geometry/linefacet/
  inline void intersect ( const osg::Vec3 &v0, const osg::Vec3& v1, std::vector< osg::Vec3 >& vertices )
  {
    osg::Vec3 diff ( v1 - v0 );

    typedef PlaneList::const_iterator Iterator;
    const PlaneList &planeList ( this->getPlaneList() );

    // Loop through each plane and see if the line segment intersects with the plane.
    for ( Iterator iter = planeList.begin(); iter != planeList.end(); ++iter )
    {
      osg::Vec4 plane ( iter->asVec4() );

      double denom ( ( plane.x() * ( v0.x() - v1.x() ) ) + ( plane.y() * ( v0.y() - v1.y() ) ) + ( plane.z() * ( v0.z() - v1.z() ) ) );

      // Line and the plane are parallel.
      if ( denom == 0.0 )
        continue;

      double num ( plane.w() + ( plane.x() * v0.x() ) + ( plane.y() * v0.y() ) + ( plane.z() * v0.z() ) );

      double u ( num / denom );

      // Line intersects, but does so past the line segment end points.
      if ( u < 0.0 || u > 1.0 )
        continue;

      osg::Vec3 point ( v0 + ( diff * u ) );

      // We may intersect with a plane, but it's outside the frustum.  Check to make sure the point is on the frustum before adding.
      // Skip the current plane, because with round off the point may evaluate to outside.
      if ( this->_contains( point, *iter ) )
        vertices.push_back ( point );
    }
  }

protected:
  inline bool _contains ( const osg::Vec3& v, const osg::Plane& skip )
  {
    typedef PlaneList::const_iterator Iterator;
    const PlaneList &planeList ( this->getPlaneList() );

    for( Iterator iter=planeList.begin(); iter!=planeList.end(); ++iter )
    {
      if ( *iter != skip )
      {
        double dist ( iter->distance(v) );

        if ( dist < 0.0 )
          return false;
      }
    }
    return true;
  }

  virtual ~Frustum ()
  {
  }

  osg::Polytope _frustum;
};

}
}

#endif // __OSGTOOLS_INTERSECT_FRUSTUM_H__

