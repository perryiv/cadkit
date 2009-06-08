
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Triangles/Loop.h"

#include "Usul/Documents/Manager.h"

#include "Usul/Interfaces/IGetDocument.h"
#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IGetVertex.h"
#include "Usul/Interfaces/ITriangulateLoop.h"
#include "Usul/Interfaces/IAddSharedVertex.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IGroup.h"

#include "osg/Matrix"
#include "osg/Plane"
#include "osg/ref_ptr"
#include "osg/BoundingSphere"
#include "osg/Version"

#include <algorithm>
#include <iostream>
#include <map>
#include <limits>

#include "Usul/Predicates/Tolerance.h"
#include "Usul/Components/Manager.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Predicates/CloseFloat.h"

#include "LoopSplitter.h"

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

    // Add up all the points.
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

  // Map to hold vec3 and the corresponding shared vertex
  typedef std::map< osg::Vec3f, SharedVertex::ValidRefPtr > Shared;

  SharedVertex*    getSharedVertex( Shared& shared, const osg::Vec3& key, Usul::Interfaces::IUnknown *caller )
  {
#if 1
    struct T
    {
      T( float t ) : _t ( t ) { }

      bool operator () ( const osg::Vec3& v1, const osg::Vec3& v2 )
      {
        Usul::Predicates::Tolerance< float > equal ( _t );

        return ( equal ( v1.x(), v2.x() ) && equal ( v1.y(), v2.y() ) && equal ( v1.z(), v2.z() ) );
      }

    private:
      float _t;
    };

    T equal ( 0.000001f );

    for ( Shared::iterator i = shared.begin(); i != shared.end(); ++i )
    {
      if( equal ( key, i->first ) )
        return i->second.get();

    }
    Shared::const_iterator iter = shared.find( key );

    // If we don't already have it, make a new one.
    if ( iter == shared.end() )
    {
      Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( caller );

      SharedVertex* sv ( addSharedVertex->addSharedVertex( key ) );

      shared.insert ( Shared::value_type ( key, sv ) );

      return sv;
    }

    // return the shared vertex.
    return iter->second.get();
#else

    Usul::Interfaces::IAddSharedVertex::ValidQueryPtr    addSharedVertex ( caller );

    SharedVertex* sv ( addSharedVertex->addSharedVertex( key ) );

    return sv;
#endif
  }


  template < class Vertices >
  void fillVertices ( Vertices &vertices, Shared& shared, Loop::Points& points, osg::Matrix& mat, Usul::Interfaces::IUnknown* caller )
  {
    typedef typename Vertices::value_type Vertex;

    Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex       ( caller );

    // Make the vertex array for the triangulate algorithm.
    for( Loop::Points::const_iterator i = points.begin(); i != points.end(); ++i )
    {
      Loop::SharedVertexPtr sv ( *i );

      // Get the vertex;
      osg::Vec3 v ( getVertex->getVertex( sv->index() ) );

      // Add the vec3 and the shared vertex to the map
      shared.insert ( Shared::value_type ( v, sv.get() ) );

      // Translate into proper plane.
      v = v * mat;

      vertices.push_back( Vertex ( v.x(), v.y() ) );

    }
  }

}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::Loop() : 
_loop(),
_innerLoops(),
_valid ( true )
{  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::Loop ( const Points &p ) : 
_loop( p.begin(), p.end() ),
_innerLoops(),
_valid ( true )
{
}

Loop::Loop( const Loop &loop ) :
_loop(loop._loop),
_innerLoops(loop._innerLoops),
_valid ( loop._valid )
{
}



///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Loop::~Loop()
{
_loop.clear();
_innerLoops.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Trianglulate loop. Add results to Document.
//
///////////////////////////////////////////////////////////////////////////////

bool Loop::triangulate ( Usul::Interfaces::IUnknown *caller, bool buildOnFly )
{
  // Useful typedefs.
  typedef Usul::Interfaces::ITriangulateLoop   Triangulate;
  typedef Triangulate::Vertices                Vertices;
  typedef Triangulate::UIntArray               UIntArray;
  typedef Triangulate::InnerLoops              InnerLoops;
  typedef Usul::Components::Manager            PluginManager;
  
  if ( this->isCoplanar(caller) == -1 ) {
    return false;
  }
  // Get needed interfaces
  Triangulate::ValidQueryPtr tri ( PluginManager::instance().getInterface( Triangulate::IID ) );
  Usul::Interfaces::IGetVertex::ValidQueryPtr                   getVertex       ( caller );
  Usul::Interfaces::IAddTriangleWithSharedVertex::ValidQueryPtr add            ( caller );
  
  // Data structures needed to triangulate.
  Vertices vertices;
  InnerLoops innerLoops;
  UIntArray indices;
  Vertices out;

  // Get the first three shared vertices
  SharedVertexPtr sv1 ( _loop.at( 0 ) );
  SharedVertexPtr sv2 ( _loop.at( 1 ) );
  SharedVertexPtr sv3 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
  osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );


  osg::Vec3 seg1 ( v2 - v1 );
  osg::Vec3 seg2 ( v3 - v2 );

  seg1.normalize();
  seg2.normalize();

  unsigned int index ( 3 );

  Usul::Predicates::CloseFloat< float > close;

  while ( close ( seg1 * seg2, 1.0 ) )
  {
    SharedVertexPtr sv ( _loop.at( index ) );
    v3 = getVertex->getVertex ( sv->index() );

    seg2 = v3 - v2;
    seg2.normalize();

    ++index;
  }
  
  //Calculate the plane that this loop is on
  osg::Plane plane ( v1, v2, v3 );

  osg::Matrix mat;

  //Make the rotation matrix to rotate the loop into the x-y plane
#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  mat.makeRotate( plane.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );
#else
  mat.makeRotate( plane.getNormal(), osg::Plane::Vec3_type ( 0.0, 0.0, 1.0 ) );
#endif

  float planeValue ( (v1 * mat).z() );

  // Local map of osg::Vec3 and the shared vertex they belong too.
  Detail::Shared shared;

  Detail::fillVertices ( vertices, shared, _loop, mat, caller );

  // Loop through the inner loops
  for ( Loop::InnerLoops::iterator i = _innerLoops.begin(); i != _innerLoops.end(); ++i )
  {
    Vertices v;
    Detail::fillVertices ( v, shared, *i, mat, caller );
    innerLoops.push_back( v );
  }

  // Triangulate.
  tri->triangulateLoop ( vertices, innerLoops, out, indices );

  // Matrix to rotate back to 3D
  osg::Matrix m;

#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  m.makeRotate( osg::Vec3 ( 0.0, 0.0, 1.0 ), plane.getNormal()  );
#else
  m.makeRotate( osg::Plane::Vec3_type ( 0.0, 0.0, 1.0 ), plane.getNormal()  );
#endif

  // Add the triangles
  for ( unsigned int i = 0; i < indices.size(); i+= 3 )
  {
    unsigned int n0 ( indices.at( i ) );
    unsigned int n1 ( indices.at( i + 1 ) );
    unsigned int n2 ( indices.at( i + 2 ) );

    osg::Vec3 v0 ( out.at( n0 )[0], out.at( n0 )[1], planeValue );
    osg::Vec3 v1 ( out.at( n1 )[0], out.at( n1 )[1], planeValue );
    osg::Vec3 v2 ( out.at( n2 )[0], out.at( n2 )[1], planeValue );

    v0 = v0 * m;
    v1 = v1 * m;
    v2 = v2 * m;

    SharedVertex *sv0 ( Detail::getSharedVertex ( shared, v0, caller ) );
    SharedVertex *sv1 ( Detail::getSharedVertex ( shared, v1, caller ) );
    SharedVertex *sv2 ( Detail::getSharedVertex ( shared, v2, caller ) );

    // Plane to get the normal.
    osg::Plane plane ( v0, v1, v2 );

    // Add the triangle.
    OsgTools::Triangles::Triangle::RefPtr t ( add->addTriangle ( sv0, sv1, sv2, plane.getNormal(), buildOnFly, true ) );

    // This triangle is not original.
    if ( t.valid() )
      t->original ( false );
  }

  // If we get here it succeeded.
  return true;
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
#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  matrix.makeRotate( planeOne.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );
#else
  matrix.makeRotate( planeOne.getNormal(), osg::Plane::Vec3_type ( 0.0, 0.0, 1.0 ) );
#endif

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
  Usul::Interfaces::IGetBoundingBox::ValidQueryPtr bbox  ( caller );
    
  osg::BoundingBox bounds (bbox->getBoundingBox() );
  // Get the osg::Vec3Array.
  osg::ref_ptr< osg::Vec3Array > vertices ( this->_vertices( caller ) );

  // Get the center point.
  center = Detail::center ( *vertices );
  
  SharedVertexPtr sv0 ( _loop.at( 0 ) );
  SharedVertexPtr sv1 ( _loop.at( 1 ) );
  SharedVertexPtr sv2 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v0 ( getVertex->getVertex ( sv0->index() ) );
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );

  int thePlane = this->isCoplanar( caller );
  osg::Vec3 normal;
  
  if ( thePlane == 0 && v0.x() == bounds.xMin() ) 
  {
      normal = osg::Vec3(1.0f, 0.0f, 0.0f);
  } 
  else if ( thePlane == 0 && v0.x() == bounds.xMax() ) 
  {
      normal = osg::Vec3(-1.0f, 0.0f, 0.0f);
  } 
  else if ( thePlane == 1 && v0.y() == bounds.yMin() ) 
  {
      normal = osg::Vec3(0.0f, 1.0f, 0.0f);  
  } 
  else if ( thePlane == 1 && v0.y() == bounds.yMax() ) 
  {
      normal = osg::Vec3(0.0f, -1.0f, 0.0f);  
  } 
  else if ( thePlane == 2 && v0.z() == bounds.zMin() ) 
  {
      normal = osg::Vec3(0.0f, 0.0f, -1.0f);  
  } 
  else if ( thePlane == 2 && v0.z() == bounds.zMax() ) 
  {
      normal = osg::Vec3(0.0f, 0.0f, 1.0f);  
  } 
  else 
  {
      // Plane to get the normal.
    osg::Plane plane ( v0, v1, v2 );
    normal = plane.getNormal();
  }

  float max ( Detail::maxDistance( *vertices, center ) );
  
  //Back up from the center
  distance = 3 * max;

  osg::Vec3 eye ( center + ( normal * distance ) );

  rotation.set( osg::Matrix::lookAt ( eye, center, normal ) );
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
// Determines if the loop is entirly contained on a single plane. If the loop
// is fully contained on a single plane, then an int value is returned that
// can be used to determine which element of the 3D vector can be safely removed
// to translate to a 2D space.
//
///////////////////////////////////////////////////////////////////////////////

int Loop::isCoplanar( Usul::Interfaces::IUnknown *caller) const 
{
  unsigned int size = _loop.size();
  int plane = -1; //0=x, 1=y, 2=z;
  float xavg = 0.0f, yavg=0.0f, zavg =0.0f;
  //Loop once to find if the loop is on a single plane.
  for( unsigned int i = 0; i < size; ++i)
  {
    xavg += (this->vertex( i, caller )).x();
    yavg += (this->vertex( i, caller )).y();
    zavg += (this->vertex( i, caller )).z();
  }
  xavg /= size;
  yavg /= size;
  zavg /= size;
  
  if (xavg == (this->vertex( 0, caller )).x() &&
      yavg != (this->vertex( 0, caller )).y() && 
      zavg != (this->vertex( 0, caller )).z() ) {
  //    std::cout << "The loop is coplanar on the ZY plane" << std::endl;
      plane = LoopSplitter::X_AXIS;
  } else if (xavg != (this->vertex( 0, caller )).x() &&
      yavg == (this->vertex( 0, caller )).y() && 
      zavg != (this->vertex( 0, caller )).z() ) {
 //     std::cout << "The loop is coplanar on the XZ plane" << std::endl;
      plane = LoopSplitter::Y_AXIS;
  } else if (xavg != (this->vertex( 0, caller )).x() &&
      yavg != (this->vertex( 0, caller )).y() && 
      zavg == (this->vertex( 0, caller )).z() ) {
 //     std::cout << "The loop is coplanar on the XY plane" << std::endl;
      plane = LoopSplitter::Z_AXIS;
  }
  return plane;

}

///////////////////////////////////////////////////////////////////////////////
//
// This will attempt to print out a conforming .poly file for the Quake
//  Triangulation program
//
///////////////////////////////////////////////////////////////////////////////

void Loop::printQuakePolygonFile( Usul::Interfaces::IUnknown *caller ) 
{
  std::cout << "#============== BEGIN POLY FILE ====================" << std::endl;
  std::cout << "# Poly file for Loop\n# This was generated from Helios Code" << std::endl;
  std::cout << this->size() << " 2 1 0" << std::endl;

  Usul::Interfaces::IGetVertex::ValidQueryPtr getVertex ( caller );

    // Get the first three shared vertices
  SharedVertexPtr sv1 ( _loop.at( 0 ) );
  SharedVertexPtr sv2 ( _loop.at( 1 ) );
  SharedVertexPtr sv3 ( _loop.at( 2 ) );
  
  //Get three vertices
  osg::Vec3 v1 ( getVertex->getVertex ( sv1->index() ) );
  osg::Vec3 v2 ( getVertex->getVertex ( sv2->index() ) );
  osg::Vec3 v3 ( getVertex->getVertex ( sv3->index() ) );


  osg::Vec3 seg1 ( v2 - v1 );
  osg::Vec3 seg2 ( v3 - v2 );

  seg1.normalize();
  seg2.normalize();

  unsigned int index ( 3 );

  Usul::Predicates::CloseFloat< float > close;

  while ( close ( seg1 * seg2, 1.0 ) )
  {
    SharedVertexPtr sv ( _loop.at( index ) );
    v3 = getVertex->getVertex ( sv->index() );

    seg2 = v3 - v2;
    seg2.normalize();

    ++index;
  }
  
  //Calculate the plane that this loop is on
  osg::Plane plane ( v1, v2, v3 );

  osg::Matrix mat;

  //Make the rotation matrix to rotate the loop into the x-y plane
#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
  mat.makeRotate( plane.getNormal(), osg::Vec3 ( 0.0, 0.0, 1.0 ) );
#else
  mat.makeRotate( plane.getNormal(), osg::Plane::Vec3_type ( 0.0, 0.0, 1.0 ) );
#endif

  for( unsigned int i = 0; i < this->size(); ++ i )
  {
    osg::Vec3 v ( this->vertex( i, caller ) );
    v = v * mat;

   // vertices->push_back( loop.vertex( i, caller ) );
    std::cout << i+1 << " " << v.x() << " " << v.y() << " 0" << std::endl; 
  }
  std::cout << this->size() << " 0" << std::endl;
  for (unsigned int i = 0; i < this->size(); ++i) {
    if (i+1 != this->size() )
      std::cout << i+1 << " " << i+1 << " " << i+2 << std::endl;
  }
  std::cout << this->size() << " " << this->size() << " 1" << std::endl;
  std::cout << "0"<< std::endl;
  std::cout << "#============== END POLY FILE ================\n\n" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Clear the loop.
//
///////////////////////////////////////////////////////////////////////////////

void Loop::clear()
{
  _loop.clear();
  _innerLoops.clear();
  _valid = true;
}
