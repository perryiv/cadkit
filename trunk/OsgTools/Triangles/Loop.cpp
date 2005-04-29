
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

#include <algorithm>

#include "Usul/Loops/FillLoop.h"
#include "Usul/Loops/Direction.h"

using namespace OsgTools::Triangles;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::Loop() : 
_loop(),
_innerLoops(),
_start ( 0 ),
_numTriangles ( 0 )
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

bool Loop::triangulate( Usul::Interfaces::IUnknown *caller )
{

  //Algorithm will fill this data structure with indices for the triangles
  Triangles triangles;

  //Triangulate
  try
  {
    this->_triangulate ( triangles, caller );
  }
  catch ( ... )
  {
    std::reverse( _loop.begin(), _loop.end() );

    this->_triangulate( triangles, caller );
  }

  //Return false if there was a problem
  if ( triangles.empty() )
    return false;

  //_start = _vertices->size();
  _numTriangles = triangles.size();

  Usul::Interfaces::IAddTriangleSharedVertex::ValidQueryPtr addTriangle ( caller );
  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

  //Add the new triangles to the vec3array
  for( unsigned int i = 0; i < triangles.size(); ++i )
  {
    int n1 ( triangles[i][0] );
    int n2 ( triangles[i][1] );
    int n3 ( triangles[i][2] );

    SharedVertexPtr sv1 ( _loop.at( n1 - 1 ) );
    SharedVertexPtr sv2 ( _loop.at( n2 - 1 ) );
    SharedVertexPtr sv3 ( _loop.at( n3 - 1 ) );
    
    //Get the vertices of this triangle. 3186504610
    osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
    osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
    osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );

    //Calculate the normal
    const osg::Vec3 t1 ( v2 - v1 );
    const osg::Vec3 t2 ( v3 - v1 );

    osg::Vec3 norm ( t1 ^ t2 );

    //Normalize the normal
    norm.normalize();

    //Add the triangle
    addTriangle->addTriangle( *sv1, *sv2, *sv3, norm );
  }

  //If we get here it succeeded
  return true;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Build points for triangulation algorithm
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
  for( std::list< Points >::iterator i = _innerLoops.begin(); i != _innerLoops.end(); ++ i )
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

  //We are probably going to need some tolerence here
  if( planeOne != planeTwo )
    return false;

  osg::Matrix matrix;

  //Make matrix to rotate to x-y plane
  matrix.makeRotate( planeOne.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );

  //Rotate the point into the proper plane
  osg::Vec3 pt ( point * matrix );

  //Vector where the data will be copied to
  osg::ref_ptr< osg::Vec3Array > copy ( new osg::Vec3Array );
  copy->reserve( _loop.size() );
  
  //Copy and shift so pt is the origin
  for( Loop::const_iterator iter = _loop.begin(); iter != _loop.end(); ++iter )
  {
    // Get the vertex
    osg::Vec3 t ( getVertex->getVertex( (*iter)->index() ) );

    //Translate point into proper plane
    osg::Vec3 v ( t * matrix );

    //Move so that pt is the origin
    v = v - pt;

    //Add the vertex to our vector
    copy->push_back( v );
  }
  
  unsigned int rCross ( 0 ); // number of right edge/ray crossings 

  // For each edge e=(i-1,i), see if crosses ray.
  for( osg::Vec3Array::const_iterator iter = copy->begin(); iter != copy->end(); ++iter ) 
  {
    // First see if q=(0,0) is a vertex. 
    if ( iter->x() == 0 && iter->y() == 0 ) 
      return false;

    osg::Vec3Array::const_iterator prev ( iter - 1 );
    if( iter == copy->begin() )
      prev = copy->end() - 1;

    // if e "straddles" the x-axis... 
    if( ( ( iter->y() > 0 ) && ( prev->y() <= 0 ) ) || ( ( prev->y() > 0 ) && ( iter->y() <= 0 ) ) ) 
    { 
      // e straddles ray, so compute intersection with ray.
      double x = ( iter->x() * prev->y() - prev->x() * iter->y() ) / ( prev->y() - iter->y() );
      
      // crosses ray if strictly positive intersection. 
      if (x > 0) 
        rCross++;
    }
  }
    
  // q inside iff an odd number of crossings.
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

  //Add up all the points
  for( Loop::const_iterator iter = _loop.begin(); iter != _loop.end(); ++iter )
  {
    SharedVertexPtr sv ( *iter );

    osg::Vec3f v ( getVertex->getVertex( sv->index() ) );

    center += v;
  }

  //Get the average for the center
  center = center / _loop.size();

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

  osg::Matrix mat;

  osg::Vec3 z (  0.0, 0.0, 1.0 );

  mat.makeRotate( z, normal );

  z = z * mat;

  //Rotate along the normal
  osg::Quat rot ( 0.0, z );
  rotation = rot;

  float max ( std::numeric_limits< float >::min() );
  
  //Find the maximium distance from the center
  for( Loop::const_iterator iter = _loop.begin(); iter != _loop.end(); ++iter )
  {
    SharedVertexPtr sv ( *iter );

    osg::Vec3 v ( getVertex->getVertex( sv->index() ) - center );
    const float t ( v.length() );
    if ( t > max )
      max = t;
  }

  //Back up from the center
  distance = 3 * max;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of planes that this loop in on
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Loop::getNumPlanes() const
{
  //TODO
  return 1;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Flip the normals that this loop created
//
///////////////////////////////////////////////////////////////////////////////

void Loop::flipNormals()
{
#if 0
  unsigned int numNormals ( _numTriangles );
  unsigned int start ( _start );

  if( _bindPerVertex )
    numNormals *= 3;
  else
    start /= 3;

  // Loop through the normals and negate them.
  std::transform ( _normals->begin() + start, _normals->begin() + start + numNormals, _normals->begin() + start, std::negate<osg::Vec3>() );

  _geometry->dirtyDisplayList();
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Undo the triangles that this loop created
//
///////////////////////////////////////////////////////////////////////////////

void Loop::undo()
{
#if 0
  //Return if we don't have any triangles to remove
  if( _numTriangles == 0 )
    return;

  //get the draw arrays
  osg::ref_ptr<osg::DrawArrays> draw ( dynamic_cast < osg::DrawArrays * > ( _geometry->getPrimitiveSet ( 0 ) ) );

  //remove the vertices
  _vertices->erase( _vertices->begin() + _start, _vertices->begin() + _start + ( _numTriangles * 3 ) );

  //remove the normals
  unsigned int numNormals ( _numTriangles );
  unsigned int normalStart ( _start );

  if( _bindPerVertex )
    numNormals *= 3;
  else
    normalStart /= 3;

  _normals->erase( _normals->begin() + normalStart, _normals->begin() + normalStart + numNormals );

  //set the new count
  draw->setCount ( _vertices->size() );

  _geometry->dirtyDisplayList();

  _start = 0;
  _numTriangles = 0;
#endif
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
