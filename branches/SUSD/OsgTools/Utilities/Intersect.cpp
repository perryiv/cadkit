
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Utilities/Intersect.h"

#include "Usul/Algorithms/ConvexHullSort.h"
#include "Usul/Math/Vector3.h"

using namespace OsgTools::Utilities;

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class used with convex-hull sort.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct VerticalVector
  {
    void operator () ( osg::Vec3 &v )
    {
      v.set  ( 0, 1, 0 ); // 3D vector.
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class used with convex-hull sort.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct VectorAngle
  {
    double operator () ( const osg::Vec3 &a, const osg::Vec3 &b )
    {
      return Usul::Math::Angle<double,3>::get ( a, b );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class used with convex-hull sort.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct PointDifference
  {
    osg::Vec3 operator () ( const osg::Vec3 &a, const osg::Vec3 &b )
    {
      return a - b;
    }
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class used with convex-hull sort.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct CompareCoordinate
  {
    CompareCoordinate ( unsigned int i )
    {
    }
    bool operator () ( const osg::Vec3 &a, const osg::Vec3 &b )
    {
      return ( a[0] < b[0] );
    }
  };
}

void OsgTools::Utilities::boundingBoxIntersect ( const osg::Plane& plane, const osg::BoundingBox& bb, osg::Vec3Array &vertices )
{
  //typedef Usul::Math::Vec3d Point;
  typedef std::vector<osg::Vec3> Polygon;
  Polygon polygon;
  polygon.reserve ( 6 );

  // Corner 0 and 1
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 0 ), bb.corner ( 1 ), point ) )
      polygon.push_back( point );
  }

  // Corner 1 and 3
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 1 ), bb.corner ( 3 ), point ) )
      polygon.push_back( point );
  }

  // Corner 3 and 2
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 3 ), bb.corner ( 2 ), point ) )
      polygon.push_back( point );
  }

  // Corner 2 and 0
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 2 ), bb.corner ( 0 ), point ) )
      polygon.push_back( point );
  }

  // Corner 0 and 4
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 0 ), bb.corner ( 4 ), point ) )
      polygon.push_back( point );
  }

  // Corner 2 and 6
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 2 ), bb.corner ( 6 ), point ) )
      polygon.push_back( point );
  }

  // Corner 1 and 5
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 1 ), bb.corner ( 5 ), point ) )
      polygon.push_back( point );
  }

  // Corner 3 and 7
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 3 ), bb.corner ( 7 ), point ) )
      polygon.push_back( point );
  }

  // Corner 7 and 5
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 7 ), bb.corner ( 5 ), point ) )
      polygon.push_back( point );
  }

  // Corner 5 and 4
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 5 ), bb.corner ( 4 ), point ) )
      polygon.push_back( point );
  }

  // Corner 4 and 6
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 4 ), bb.corner ( 6 ), point ) )
      polygon.push_back( point );
  }

  // Corner 6 and 7
  {
    osg::Vec3 point;
    if( lineSegmentIntersect ( plane, bb.corner( 6 ), bb.corner ( 7 ), point ) )
      polygon.push_back( point );
  }

  typedef Detail::VerticalVector VerticalVector;
  typedef Detail::CompareCoordinate CompareCoordinate;
  typedef Detail::VectorAngle VectorAngle;
  typedef Detail::PointDifference PointDifference;
  typedef Usul::Algorithms::ConvexHullSort<Polygon,VerticalVector,CompareCoordinate,VectorAngle,PointDifference> ConvexHullSort;

  ConvexHullSort::sort ( polygon );

  vertices.resize( polygon.size() );
  std::copy ( polygon.begin(), polygon.end(), vertices.begin() );
}
