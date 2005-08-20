
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Loop.h"

#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IGetVertex.h"

#include "osg/Matrix"
#include "osg/Plane"
#include "osg/ref_ptr"
#include "osg/BoundingSphere"

#include <algorithm>

#include "Usul/Loops/FillLoop.h"
#include "Usul/Loops/Direction.h"
#include "Usul/Predicates/Tolerance.h"

using namespace OsgTools::Triangles;

namespace Detail
{
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return the center of the vec3array.
  //
  ///////////////////////////////////////////////////////////////////////////////

  osg::Vec3 center ( const osg::Vec3Array & vertices )
  {
    osg::Vec3 center;

    //Add up all the points
    for( osg::Vec3Array::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
       center += *iter;
    }

    //Get the average for the center
    center = center / vertices.size();

    return center;
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Return the maximium distance from the center.
  //
  ///////////////////////////////////////////////////////////////////////////////

  float maxDistance ( const osg::Vec3Array & vertices, const osg::Vec3 & center )
  {
    float max ( std::numeric_limits< float >::min() );
  
    //Find the maximium distance from the center
    for( osg::Vec3Array::const_iterator iter = vertices.begin(); iter != vertices.end(); ++iter )
    {
      osg::Vec3 d ( *iter - center );
      const float t ( d.length() );
      if ( t > max )
        max = t;
    }

    return max;
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::Loop() : 
_loop(),
_innerLoops()
{  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::Loop ( const Points &p ) : 
_loop( p.begin(), p.end() ),
_innerLoops()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::~Loop()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Trianglulate loop.  Add results to Document
//
///////////////////////////////////////////////////////////////////////////////

bool Loop::triangulate( Usul::Interfaces::IUnknown *caller, bool buildOnFly  )
{
  // Algorithm will fill this data structure with indices for the triangles.
  Triangles triangles;

  // Triangulate
  try
  {
    this->_triangulate ( triangles, caller );
  }
  catch ( ... )
  {
    std::reverse( _loop.begin(), _loop.end() );

    this->_triangulate( triangles, caller );
  }

  // Return false if there was a problem.
  if ( triangles.empty() )
    return false;

  Usul::Interfaces::IAddTriangleSharedVertex::ValidQueryPtr addTriangle ( caller );
  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  // Add the new triangles to the vec3array.
  for( unsigned int i = 0; i < triangles.size(); ++i )
  {
    int n1 ( triangles[i][0] );
    int n2 ( triangles[i][1] );
    int n3 ( triangles[i][2] );

    SharedVertexPtr sv1 ( _loop.at( n1 - 1 ) );
    SharedVertexPtr sv2 ( _loop.at( n2 - 1 ) );
    SharedVertexPtr sv3 ( _loop.at( n3 - 1 ) );
    
    // Get the vertices of this triangle. 3186504610
    osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
    osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
    osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );

    // Calculate the normal.
    const osg::Vec3 t1 ( v2 - v1 );
    const osg::Vec3 t2 ( v3 - v1 );

    osg::Vec3 norm ( t1 ^ t2 );

    // Normalize the normal.
    norm.normalize();

    // Add the triangle.
    addTriangle->addTriangle( *sv1, *sv2, *sv3, norm, true, buildOnFly );
  }

  // If we get here it succeeded.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build points for triangulation algorithm.
//
///////////////////////////////////////////////////////////////////////////////

void Loop::_buildPoints ( std::list< unsigned int > &sizes, osg::Vec2Array &points, Usul::Interfaces::IUnknown *caller  )
{
  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  SharedVertexPtr sv1 ( _loop.at( 0 ) );
  SharedVertexPtr sv2 ( _loop.at( 1 ) );
  SharedVertexPtr sv3 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
  osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );
  
  //Calculate the plane that this loop is on
  osg::Plane plane ( v1, v2, v3 );

  osg::Matrix mat;

  //Make the rotation matrix to rotate the loop into the x-y plane
  mat.makeRotate( plane.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );

  //first point must be a dummy point for algorithm 3186504610
  points.push_back ( osg::Vec2( 0.0, 0.0 ) );

  //Add the size of this loop to sizes
  sizes.push_back( _loop.size() );

  for( const_iterator i = _loop.begin(); i != _loop.end(); ++i )
  {
    SharedVertexPtr sv ( *i );

    osg::Vec3 v ( getVertex->getVertex( sv->index() ) );

    v = v * mat;

    points.push_back ( osg::Vec2 ( v[0], v[1] ) );

  }

  //Reverse the loop if it is not counter clockwise
  if( !Usul::Loops::isCounterClockwise( points.begin(), points.end() - 1 ) )
  {
    std::reverse( points.begin(), points.end() );
  }

  //Go through any inner loops
  for( std::vector< Points >::iterator i = _innerLoops.begin(); i != _innerLoops.end(); ++ i )
  {
    osg::ref_ptr <osg::Vec2Array> tPoints ( new osg::Vec2Array );

    sizes.push_back( i->size() );

    for( const_iterator j = i->begin(); j != i->end(); ++j )
    {
      SharedVertexPtr sv ( *j );

      osg::Vec3 v ( getVertex->getVertex( sv->index() ) );

      v = v * mat;

      tPoints->push_back ( osg::Vec2 ( v[0], v[1] ) );
    }

    //Inner loops need to be clock-wise
    if( Usul::Loops::isCounterClockwise( tPoints->begin(), tPoints->end() - 1 ) )
    {
      std::reverse( tPoints->begin(), tPoints->end() );
      std::reverse( i->begin(), i->end() );
    }

    points.insert( points.end(), tPoints->begin(), tPoints->end() );
    _loop.insert( _loop.end(), i->begin(), i->end() );

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Triangulate
//
///////////////////////////////////////////////////////////////////////////////

void Loop::_triangulate ( Triangles &triangles, Usul::Interfaces::IUnknown *caller )
{
  //Clear in case any thing was added before
  triangles.clear();

  //reserve enough room
  triangles.reserve( _loop.size() );

  std::list< unsigned int > sizes;
  
  osg::ref_ptr < osg::Vec2Array > points ( new osg::Vec2Array );
  points->reserve( _loop.size() + 1 );

  this->_buildPoints( sizes, *points, caller );

  Usul::Loops::triangulate_polygon( sizes, *points, triangles );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if inside, false if anything else ( including on the polygon )
//  Adapted from "Computational Geometry in C" (Second Edition) by Joseph O'Rourke
//
///////////////////////////////////////////////////////////////////////////////

bool Loop::pointInside( const osg::Vec3 &point, Usul::Interfaces::IUnknown *caller ) const
{
  //This shouldn't be true, but just in case...
  if( _loop.size() < 3 )
    return false;

  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  SharedVertexPtr sv1 ( _loop.at( 0 ) );
  SharedVertexPtr sv2 ( _loop.at( 1 ) );
  SharedVertexPtr sv3 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
  osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );

  //first check to see if point is in the same plane
  osg::Plane planeOne ( v1, v2, v3 );
  osg::Plane planeTwo ( v1, v2, point );

  osg::Vec3 n1 ( planeOne.getNormal() );
  osg::Vec3 n2 ( planeTwo.getNormal() );

  float d ( planeOne.distance( point ) );

  //We are probably going to need some tolerence here
  if( ( n1 * n2 ) > 0.25 && ( n1 * -n2 ) > 0.25 || ( d > 5.0 || d < -5.0 ) )
    return false;

  osg::Matrix matrix;

  //Make matrix to rotate to x-y plane
  matrix.makeRotate( planeOne.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );

  //Rotate the point into the proper plane
  osg::Vec3 pt ( point * matrix );

  // The vertices of this loop
  osg::ref_ptr< osg::Vec3Array > vertices ( this->_vertices( caller ) );

  //Copy and shift so pt is the origin
  for( osg::Vec3Array::iterator iter = vertices->begin(); iter != vertices->end(); ++iter )
  {
    //Translate point into proper plane
    *iter = *iter * matrix;

    //Move so that pt is the origin
    *iter -= pt;
  }
  
  unsigned int rCross ( 0 ); // number of right edge/ray crossings 

  // For each edge e=(i-1,i), see if crosses ray.
  for( osg::Vec3Array::const_iterator iter = vertices->begin(); iter != vertices->end(); ++iter ) 
  {
    // First see if q=(0,0) is a vertex. 
    if ( iter->x() == 0 && iter->y() == 0 ) 
      return false;

    osg::Vec3Array::const_iterator prev ( iter - 1 );
    if( iter == vertices->begin() )
      prev = vertices->end() - 1;

    // if e "straddles" the x-axis... 
    if( ( ( iter->y() > 0 ) && ( prev->y() <= 0 ) ) || ( ( prev->y() > 0 ) && ( iter->y() <= 0 ) ) ) 
    //if( ( iter->y() > 0 ) != ( prev->y() > 0 ) ) 
    { 
      // e straddles ray, so compute intersection with ray.
      double x = ( iter->x() * prev->y() - prev->x() * iter->y() ) / ( prev->y() - iter->y() );
      
      // crosses ray if strictly positive intersection. 
      if (x > 0) 
        rCross++;
    }
  }
    
  // point inside iff an odd number of crossings.
  if( (rCross % 2) == 1 )
    return true;

  //if we get here return false
  return false;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the data needed to create a frame
//
///////////////////////////////////////////////////////////////////////////////

void Loop::getFrameData ( osg::Vec3& center, float &distance, osg::Quat& rotation, Usul::Interfaces::IUnknown *caller ) const
{  
  if( _loop.size() < 3 )
    return;

  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  // Get the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( this->_vertices( caller ) );

  // Get the center point.
  center = Detail::center ( *vertices );

  SharedVertexPtr sv1 ( _loop.at( 0 ) );
  SharedVertexPtr sv2 ( _loop.at( 1 ) );
  SharedVertexPtr sv3 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
  osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );

  //Calculate the normal
  const osg::Vec3 t1 ( v2 - v1 );
  const osg::Vec3 t2 ( v3 - v1 );

  osg::Vec3 normal ( t1 ^ t2 );

  //Normalize the normal
  normal.normalize();

  float max ( Detail::maxDistance( *vertices, center ) );
  
  //Back up from the center
  distance = 3 * max;

  osg::Vec3 eye ( center + ( normal * distance ) );

  osg::Matrix mat;

  mat.makeLookAt( eye, center, normal );

  rotation.set( mat );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ith vertex
//
///////////////////////////////////////////////////////////////////////////////

const osg::Vec3f& Loop::vertex ( unsigned int i, Usul::Interfaces::IUnknown *caller  ) const
{
  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  SharedVertexPtr sv ( _loop.at( i ) );

  return getVertex->getVertex( sv->index() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the osg::Vec3array from the loop.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3Array* Loop::_vertices( Usul::Interfaces::IUnknown *caller ) const
{
  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  vertices->reserve( this->size() );

  // Build the vec3array
  for( Loop::const_iterator iter = _loop.begin(); iter != _loop.end(); ++iter )
  {
    SharedVertexPtr sv ( *iter );

    osg::Vec3f v ( getVertex->getVertex( sv->index() ) );

    vertices->push_back ( v );
  }

  return vertices.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes that the loop covers.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Loop::numPlanes ( Usul::Interfaces::IUnknown* caller ) const
{
  typedef Usul::Math::Vector3< bool > Vec3b;

  Vec3b changed ( true, true, true );

  // The vertices of this loop
  osg::ref_ptr< osg::Vec3Array > vertices ( this->_vertices( caller ) );

  Usul::Predicates::Tolerance< float > equal ( 0.5 );

  for( osg::Vec3Array::const_iterator iter = vertices->begin(); iter != vertices->end(); ++iter ) 
  {
    Vec3b c ( false, false, false );

    osg::Vec3Array::const_iterator prev ( iter - 1 );
    if( iter == vertices->begin() )
      prev = vertices->end() - 1;

    // Has x changed?
    if( false == equal ( prev->x(), iter->x() ) )
      c[0] = true;

    if( false == equal ( prev->y(), iter->y() ) )
      c[1] = true;

    if( false == equal ( prev->z(), iter->z() ) )
      c[2] = true;

    changed[0] = changed[0] && c[0];
    changed[1] = changed[1] && c[1];
    changed[2] = changed[2] && c[2];
  }

  unsigned int count ( 0 );

  if( false == changed[0] )
    ++count;

  if( false == changed[1] )
    ++count;

  if( false == changed[2] )
    ++count;

  //return count;
  return 1.0;
}

