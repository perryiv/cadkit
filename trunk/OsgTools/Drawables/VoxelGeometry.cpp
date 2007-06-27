
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Geometry for voxel models.
//
///////////////////////////////////////////////////////////////////////////////

#include "VoxelGeometry.h"

#include "OsgTools/Convert.h"

#include "Usul/Errors/Assert.h"
#include "Usul/Algorithms/ConvexHullSort.h"
#include "Usul/Scope/GLBeginEnd.h"
#include "Usul/Exceptions/Thrower.h"

#include "osg/Notify"

using namespace OsgTools::Drawables;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VoxelGeometry::VoxelGeometry() : BaseClass(),
  _numPlanes ( 3 ),
  _bbox      ( osg::Vec3 ( 0, 0, 0 ), osg::Vec3 ( 5, 1, 5 ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

VoxelGeometry::VoxelGeometry ( const VoxelGeometry &d, const osg::CopyOp &options ) : 
  BaseClass  ( d, options   ),
  _numPlanes ( d._numPlanes ),
  _bbox      ( d._bbox      )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment
//
///////////////////////////////////////////////////////////////////////////////

VoxelGeometry &VoxelGeometry::operator = ( const VoxelGeometry &d )
{
  _numPlanes = d._numPlanes;
  _bbox      = d._bbox;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

VoxelGeometry::~VoxelGeometry()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the borders.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_initBorders ( VoxelGeometry::Lines &b ) const
{
  b.resize ( 12 );
  unsigned int i ( 0 );
  const Vec3 mn ( _bbox.xMin(), _bbox.yMin(), _bbox.zMin() );
  const Vec3 mx ( _bbox.xMax(), _bbox.yMax(), _bbox.zMax() );

  b.at(i).first.first.set ( mn[0], mn[1], mn[2] ); b.at(i).second.first.set ( mx[0], mn[1], mn[2] ); b.at(i).first.second.set ( 0, 0, 0 ) ; b.at(i).second.second.set ( 1, 0, 0 ); ++i;
  b.at(i).first.first.set ( mx[0], mn[1], mn[2] ); b.at(i).second.first.set ( mx[0], mx[1], mn[2] ); b.at(i).first.second.set ( 1, 0, 0 ) ; b.at(i).second.second.set ( 1, 1, 0 ); ++i;
  b.at(i).first.first.set ( mx[0], mx[1], mn[2] ); b.at(i).second.first.set ( mn[0], mx[1], mn[2] ); b.at(i).first.second.set ( 1, 1, 0 ) ; b.at(i).second.second.set ( 0, 1, 0 ); ++i;
  b.at(i).first.first.set ( mn[0], mx[1], mn[2] ); b.at(i).second.first.set ( mn[0], mn[1], mn[2] ); b.at(i).first.second.set ( 0, 1, 0 ) ; b.at(i).second.second.set ( 0, 0, 0 ); ++i;

  b.at(i).first.first.set ( mn[0], mn[1], mx[2] ); b.at(i).second.first.set ( mx[0], mn[1], mx[2] ); b.at(i).first.second.set ( 0, 0, 1 ) ; b.at(i).second.second.set ( 1, 0, 1 ); ++i;
  b.at(i).first.first.set ( mx[0], mn[1], mx[2] ); b.at(i).second.first.set ( mx[0], mx[1], mx[2] ); b.at(i).first.second.set ( 1, 0, 1 ) ; b.at(i).second.second.set ( 1, 1, 1 ); ++i;
  b.at(i).first.first.set ( mx[0], mx[1], mx[2] ); b.at(i).second.first.set ( mn[0], mx[1], mx[2] ); b.at(i).first.second.set ( 1, 1, 1 ) ; b.at(i).second.second.set ( 0, 1, 1 ); ++i;
  b.at(i).first.first.set ( mn[0], mx[1], mx[2] ); b.at(i).second.first.set ( mn[0], mn[1], mx[2] ); b.at(i).first.second.set ( 0, 1, 1 ) ; b.at(i).second.second.set ( 0, 0, 1 ); ++i;

  b.at(i).first.first.set ( mn[0], mn[1], mn[2] ); b.at(i).second.first.set ( mn[0], mn[1], mx[2] ); b.at(i).first.second.set ( 0, 0, 0 ) ; b.at(i).second.second.set ( 0, 0, 0 ); ++i;
  b.at(i).first.first.set ( mx[0], mn[1], mn[2] ); b.at(i).second.first.set ( mx[0], mn[1], mx[2] ); b.at(i).first.second.set ( 1, 0, 0 ) ; b.at(i).second.second.set ( 1, 0, 0 ); ++i;
  b.at(i).first.first.set ( mn[0], mx[1], mn[2] ); b.at(i).second.first.set ( mn[0], mx[1], mx[2] ); b.at(i).first.second.set ( 0, 1, 0 ) ; b.at(i).second.second.set ( 0, 1, 0 ); ++i;
  b.at(i).first.first.set ( mx[0], mx[1], mn[2] ); b.at(i).second.first.set ( mx[0], mx[1], mx[2] ); b.at(i).first.second.set ( 1, 1, 0 ) ; b.at(i).second.second.set ( 1, 1, 0 ); ++i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the bounding-box.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_initBoundingBox ( const Matrix44 &mv, Lines &borders, osg::BoundingBox &box ) const
{
  // Loop through the lines.
  for ( Lines::iterator i = borders.begin(); i != borders.end(); ++i )
  {
    // Put vertices in world space.
    i->first.first.set  ( mv * i->first.first  );
    i->second.first.set ( mv * i->second.first );

    // Make sure first vertex has smallest z. This is important below.
    if ( i->second.first[2] < i->first.first[2] )
    {
      Point temp ( i->first );
      i->first = i->second;
      i->second = temp;
    }

    // Expand the bounding box of the vertices.
    box.expandBy ( osg::Vec3f ( i->first.first[0],  i->first.first[1],  i->first.first[2]  ) );
    box.expandBy ( osg::Vec3f ( i->second.first[0], i->second.first[1], i->second.first[2] ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Compute the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox VoxelGeometry::computeBound() const
{
  return _bbox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overload of osg::Object::cloneType()
//
///////////////////////////////////////////////////////////////////////////////

osg::Object *VoxelGeometry::cloneType() const
{
  return new VoxelGeometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overload of osg::Object::clone()
//
///////////////////////////////////////////////////////////////////////////////

osg::Object *VoxelGeometry::clone ( const osg::CopyOp &options ) const
{
  return new VoxelGeometry ( *this, options );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to clamp the vector into range.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T1, class T2 > void _clamp ( const T1 &lower, const T1 &upper, T2 &v )
  {
    USUL_ASSERT ( lower < upper );
    if ( v < lower )
      v = lower;
    if ( v > upper )
      v = upper;
  }
  template < class V > void _clamp ( const osg::BoundingBox &b, V &v )
  {
    Detail::_clamp ( b.xMin(), b.xMax(), v[0] );
    Detail::_clamp ( b.yMin(), b.yMax(), v[1] );
    Detail::_clamp ( b.zMin(), b.zMax(), v[2] );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Intersect the plane with the borders. Border points are in world space.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_intersect ( const Matrix44 &imv, const Lines &borders, float z, Polygon &polygon ) const
{
  // Initialize.
  polygon.resize ( 0 );
  const osg::BoundingBox tbox ( osg::Vec3f ( 0, 0, 0 ), osg::Vec3f ( 1, 1, 1 ) );

  // Loop through the borders.
  for ( Lines::const_iterator i = borders.begin(); i != borders.end(); ++i )
  {
    // Is this line on both sides of the plane?
    if ( i->first.first[2] < z && i->second.first[2] > z )
    {
      // Intersect the line and the plane. Since the plane is 
      // axis-aligned, it is just a linear interpolation.
      const double fraction ( ( z - i->first.first[2] ) / ( i->second.first[2] - i->first.first[2] ) );
      Vec3 xyz ( i->first.first + ( i->second.first - i->first.first ) * fraction );

      // Convert to local space.
      xyz = imv * xyz;

      // Texture coordinate is also a linear blend.
      Vec3 str ( i->first.second + ( i->second.second - i->first.second ) * fraction );

      // Clamp to range. Have to do this because of matrix-vector roundoff.
      Detail::_clamp ( _bbox, xyz );
      Detail::_clamp (  tbox, str );

      // Save in container.
      polygon.push_back ( Point ( xyz, str ) );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the voxels.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::drawImplementation( osg::RenderInfo& renderInfo ) const
{
  try
  {
    this->_drawImplementation ( *renderInfo.getState() );
  }

  catch ( const std::exception &e )
  {
    const std::string message ( ( e.what() ) ? e.what() : "Error 2557421800: standard exception caught" );
    osg::notify ( osg::WARN ) << message.c_str() << std::endl;
  }

  catch ( ... )
  {
    osg::notify ( osg::WARN ) << "Error 3574188933: unknown exception caught" << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the voxels.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_drawImplementation ( osg::State &state ) const
{
  // Get the model-view matrix and its inverse.
  Matrix44 mv, imv;
  OsgTools::Convert::matrix ( state.getModelViewMatrix(), mv );
  if ( !mv.inverse ( imv ) )
    throw std::runtime_error ( "Error 1349735105: failed to find inverse of model-view matrix" );

  // Initialize the borders in local space.
  Lines borders;
  this->_initBorders ( borders );

  // Puts both the bounding-box and the borders in world-space.
  osg::BoundingBox bbox;
  this->_initBoundingBox ( mv, borders, bbox );

  // Make the polygons in local space. Each one is a plane.
  Polygons polygons;
  this->_makePolygons ( imv, bbox, borders, polygons );

  // Draw the bounding-box. Used for debugging.
  this->_drawLines ( imv, borders );

  // Draw the planes.
  this->_drawPolygons ( polygons );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class used with convex-hull sort.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct VerticalVector
  {
    void operator () ( VoxelGeometry::Point &v )
    {
      v.first.set  ( 0, 1, 0 ); // 3D vector.
      v.second.set ( 0, 0, 0 ); // Texture coordinate, just along for the ride.
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
    double operator () ( const VoxelGeometry::Point &a, const VoxelGeometry::Point &b )
    {
      return Usul::Math::Angle<double,3>::get ( a.first, b.first );
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
    VoxelGeometry::Point operator () ( const VoxelGeometry::Point &a, const VoxelGeometry::Point &b )
    {
      return VoxelGeometry::Point ( a.first - b.first, a.second - b.second );
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
    bool operator () ( const VoxelGeometry::Point &a, const VoxelGeometry::Point &b )
    {
      return ( a.first[0] < b.first[0] );
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the polygons by intersecting with the planes.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_makePolygons ( const Matrix44 &imv, const osg::BoundingBox &bbox, const Lines &borders, Polygons &polygons ) const
{
  typedef Detail::VerticalVector VerticalVector;
  typedef Detail::CompareCoordinate CompareCoordinate;
  typedef Detail::VectorAngle VectorAngle;
  typedef Detail::PointDifference PointDifference;
  typedef Usul::Algorithms::ConvexHullSort<Polygon,VerticalVector,CompareCoordinate,VectorAngle,PointDifference> ConvexHullSort;

  // A single polygon that defines one plane.
  Polygon polygon;
  polygon.reserve ( 6 );

  // Reserve space in the container of polygons.
  polygons.reserve ( _numPlanes );

  // Loop through the planes.
  for ( unsigned int j = 0; j < _numPlanes; ++j )
  {
    // Get the z coordinate.
    float z ( ( float ( j ) ) / ( float ( _numPlanes - 1 ) ) );
    z = bbox.zMin() + z * ( bbox.zMax() - bbox.zMin() );

    // Intersect the plane with the border. Puts the points in local space.
    this->_intersect ( imv, borders, z, polygon );

    // Should be true.
    USUL_ASSERT ( polygon.empty() || ( ( polygon.size() > 2 ) && ( polygon.size() < 7 ) ) );

    // The points define the convex-hull. Need a counter-clockwise loop.
    ConvexHullSort::sort ( polygon );

    // Save in the container.
    polygons.push_back ( polygon );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw all the polygons.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_drawPolygons ( const Polygons &p ) const
{
  for ( Polygons::const_iterator i = p.begin(); i != p.end(); ++i )
    this->_drawPolygon ( *i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw a point.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_drawPoint ( const VoxelGeometry::Point &p ) const
{
#if 1

  if ( false == _bbox.contains ( osg::Vec3f ( p.first[0],  p.first[1],  p.first[2] ) ) )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 4155263366: vertex coordinates are out of range: ", p.first[0], ", ", p.first[1], ", ", p.first[2] );

  const osg::BoundingBox bbox ( osg::Vec3f ( 0, 0, 0 ), osg::Vec3f ( 1, 1, 1 ) );
  if ( false == bbox.contains ( osg::Vec3f ( p.second[0],  p.second[1],  p.second[2] ) ) )
    Usul::Exceptions::Thrower<std::runtime_error> ( "Error 4155263366: texture coordinates are out of range: ", p.second[0], ", ", p.second[1], ", ", p.second[2] );

#endif

  //::glTexCoord3dv ( p.second.get() );
  ::glVertex3dv   ( p.first.get()  );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the polygon.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_drawPolygon ( const Polygon &p ) const
{
  switch ( p.size() )
  {
    case 3:
    {
      Usul::Scope::GLBeginEnd be ( GL_TRIANGLES );
      this->_drawPoint ( p[0] );
      this->_drawPoint ( p[1] );
      this->_drawPoint ( p[2] );
      break;
    }
    case 4:
    {
      Usul::Scope::GLBeginEnd be ( GL_TRIANGLE_STRIP );
      this->_drawPoint ( p[0] );
      this->_drawPoint ( p[1] );
      this->_drawPoint ( p[3] );
      this->_drawPoint ( p[2] );
      break;
    }
    case 5:
    {
      Usul::Scope::GLBeginEnd be ( GL_TRIANGLE_FAN );
      this->_drawPoint ( p[0] );
      this->_drawPoint ( p[1] );
      this->_drawPoint ( p[2] );
      this->_drawPoint ( p[3] );
      this->_drawPoint ( p[4] );
      break;
    }
    case 6:
    {
      Usul::Scope::GLBeginEnd be ( GL_TRIANGLE_FAN );
      this->_drawPoint ( p[0] );
      this->_drawPoint ( p[1] );
      this->_drawPoint ( p[2] );
      this->_drawPoint ( p[3] );
      this->_drawPoint ( p[4] );
      this->_drawPoint ( p[5] );
      break;
    }
    default:
    {
      USUL_ASSERT ( 0 == p.size() );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the lines. The lines are in world space.
//
///////////////////////////////////////////////////////////////////////////////

void VoxelGeometry::_drawLines ( const Matrix44 &imv, const Lines &lines ) const
{
  Usul::Scope::GLBeginEnd be ( GL_LINES );
  for ( Lines::const_iterator i = lines.begin(); i != lines.end(); ++i )
  {
    // Put in local space.
    const Vec3 a ( imv * i->first.first  );
    const Vec3 b ( imv * i->second.first );

    // Draw the line.
    ::glVertex3dv ( a.get() );
    ::glVertex3dv ( b.get() );
  }
}
