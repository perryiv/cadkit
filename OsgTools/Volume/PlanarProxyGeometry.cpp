
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Algorithm to build the planes adapted from "Real-Time Volume Graphics"
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Volume/PlanarProxyGeometry.h"

#include "Usul/Functions/Color.h"

#include "osg/Notify"

using namespace OsgTools::Volume;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PlanarProxyGeometry::PlanarProxyGeometry() : 
  BaseClass(),
  _numPlanes ( 8 ),
  _bbox      ( osg::Vec3 ( -1, -1, -1 ), osg::Vec3 ( 1, 1, 1 ) ),
  _corners   ( 8 ),
  _edges     ( 12 ),
  _vertices  (),
  _indices   ( 6 )
{
  this->_initCornersAndEdges();
  this->setUseDisplayList( false );

  for ( unsigned int i = 0; i < 6; ++i )
    _indices[ i ] = i;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

PlanarProxyGeometry::PlanarProxyGeometry ( const PlanarProxyGeometry &d, const osg::CopyOp &options ) : 
  BaseClass  ( d, options   ),
  _numPlanes ( d._numPlanes ),
  _bbox      ( d._bbox      ),
  _corners   ( d._corners   ),
  _edges     ( d._edges     ),
  _vertices  ( d._vertices  ),
  _indices   ( d._indices   )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment
//
///////////////////////////////////////////////////////////////////////////////

PlanarProxyGeometry &PlanarProxyGeometry::operator = ( const PlanarProxyGeometry &d )
{
  _numPlanes = d._numPlanes;
  _bbox      = d._bbox;
  _corners   = d._corners;
  _edges     = d._edges;
  _vertices  = d._vertices;
  _indices   = d._indices;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

PlanarProxyGeometry::~PlanarProxyGeometry()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overload of osg::Object::cloneType()
//
///////////////////////////////////////////////////////////////////////////////

osg::Object *PlanarProxyGeometry::cloneType() const
{
  return new PlanarProxyGeometry;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overload of osg::Object::clone()
//
///////////////////////////////////////////////////////////////////////////////

osg::Object *PlanarProxyGeometry::clone ( const osg::CopyOp &options ) const
{
  return new PlanarProxyGeometry ( *this, options );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize the corners and edges.
//
///////////////////////////////////////////////////////////////////////////////

void PlanarProxyGeometry::_initCornersAndEdges ()
{
  osg::Vec3 min ( _bbox._min );
  osg::Vec3 max ( _bbox._max );

  // The corners.
  _corners[0] = min;
	_corners[1] = osg::Vec3f( max.x(), min.y(), min.z() );
	_corners[2] = osg::Vec3f( max.x(), max.y(), min.z() );
	_corners[3] = osg::Vec3f( min.x(), max.y(), min.z() );

	_corners[4] = osg::Vec3f( min.x(), min.y(), max.z() );
	_corners[5] = osg::Vec3f( max.x(), min.y(), max.z() );
	_corners[6] = max;
	_corners[7] = osg::Vec3f( min.x(), max.y(), max.z() );

  // Edge connectivity.
	_edges[0]  = Edge( 0,1 );
	_edges[1]  = Edge( 1,2 );
	_edges[2]  = Edge( 2,3 );
	_edges[3]  = Edge( 3,0 );
	_edges[4]  = Edge( 0,4 );
	_edges[5]  = Edge( 1,5 );
	_edges[6]  = Edge( 2,6 );
	_edges[7]  = Edge( 3,7 );
	_edges[8]  = Edge( 4,5 );
	_edges[9]  = Edge( 5,6 );
	_edges[10] = Edge( 6,7 );
	_edges[11] = Edge( 7,4 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the proxy geometry.
//
///////////////////////////////////////////////////////////////////////////////

void PlanarProxyGeometry::drawImplementation( DrawArgs ) const
{
  try
  {
#if OSG_VERSION_MAJOR <= 1 && OSG_VERSION_MINOR <= 2
    this->_drawImplementation ( state );
#else
    this->_drawImplementation ( *info.getState() );
#endif
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
//  Edge connectivity that constitute a path. 
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
	const unsigned int edgeList[8][12] = 
  {
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 
}

///////////////////////////////////////////////////////////////////////////////
//
//  Helper class to push and pop the state.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class PushPopStateSet
  {
  public:
    PushPopStateSet ( const osg::StateSet* ss, osg::State& state ) :
    _state ( state )
    {
      _state.pushStateSet ( ss );
      //::glEnableClientState ( GL_VERTEX_ARRAY );
    }
    ~PushPopStateSet ()
    {
      //::glDisableClientState ( GL_VERTEX_ARRAY );
      _state.popStateSet();
    }

  private:
    osg::State & _state;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Draw the proxy geometry.
//
///////////////////////////////////////////////////////////////////////////////

void PlanarProxyGeometry::_drawImplementation( osg::State& state ) const
{
  // Apply the state.
  //Detail::PushPopStateSet pushPopState ( this->getStateSet(), state );

  // Get the vector of the view direction.
  osg::Matrix matrix ( state.getModelViewMatrix() );
  osg::Vec4f  viewVec ( -matrix( 0, 2 ),-matrix( 1, 2 ), -matrix( 2, 2 ), 0.0 );

  // Min and Max distance of the box from the eye point.
	double maxDistance ( viewVec * _corners[0] );
	double minDistance ( maxDistance );

  // Index of the vertex that is closest to the eye point.
	unsigned int frontId ( 0 );

  // Loop through the points of the box and find which point is closet to the eye.
  for( unsigned int i = 1; i < 8; ++i ) 
  {
		double distance ( viewVec * _corners[ i ] );
		
    if ( distance > maxDistance )
    {
			maxDistance = distance;
			frontId = i;
		}
		if ( distance < minDistance ) 
    {
			minDistance = distance;
		}
	}

  double planeDistance    ( minDistance );
	double planeIncrement   ( ( maxDistance - minDistance ) / _numPlanes );

  float lambda      [ 12 ];
	float lambdaInc   [ 12 ];
	
  osg::Vec3f vecStart [ 12 ];
	osg::Vec3f vecDir   [ 12 ];

  // Loop through the number of edges.
	for( unsigned int i = 0; i < 12; ++i )
	{ 
    // Get the edge.
    const Edge &edge ( _edges[ Detail::edgeList[ frontId ][ i ] ] );

    // Get the corner and vector along the edge.
		vecStart[i] = _corners[ edge.first ];
		vecDir[i]   = _corners[ edge.second ] - _corners[ edge.first ];

		double d ( vecDir[i] * viewVec );

		if ( 0.0 != d )
		{
			lambdaInc[ i ] =  planeIncrement / d;
			lambda[ i ]    = ( planeDistance - vecStart[ i ] * viewVec ) / d;
		} 
    else 
    {
      lambdaInc[ i ] =  0.0;
			lambda[ i ] = -1.0;
			
		}
	}

  float lmb [ 12 ];
  

  // Create the slices.
	for( int n = _numPlanes - 1; n >= 0; --n ) 
	{
    Vertices& intersection ( const_cast < Vertices& > ( _vertices.at ( n ) ) );

		for( int e = 0; e < 12; e++ ) 
    {
			lmb[ e ] = lambda[ e ] + n * lambdaInc[ e ];
		}

		if      ( ( lmb[ 0 ] >= 0.0 ) && ( lmb[ 0 ] < 1.0 ) ) intersection[ 0 ] = vecStart[ 0 ] + vecDir[ 0 ] * lmb[ 0 ] ;
		else if ( ( lmb[ 1 ] >= 0.0 ) && ( lmb[ 1 ] < 1.0 ) ) intersection[ 0 ] = vecStart[ 1 ] + vecDir[ 1 ] * lmb[ 1 ] ;
		else if ( ( lmb[ 3 ] >= 0.0 ) && ( lmb[ 3 ] < 1.0 ) ) intersection[ 0 ] = vecStart[ 3 ] + vecDir[ 3 ] * lmb[ 3 ];
		else continue;
		
		if	    ( ( lmb[ 2 ] >= 0.0 ) && ( lmb[ 2 ] < 1.0 ) ) intersection[ 1 ] = vecStart[ 2 ] + vecDir[ 2 ] * lmb[ 2 ];
		else if ( ( lmb[ 0 ] >= 0.0 ) && ( lmb[ 0 ] < 1.0 ) ) intersection[ 1 ] = vecStart[ 0 ] + vecDir[ 0 ] * lmb[ 0 ];
		else if ( ( lmb[ 1 ] >= 0.0 ) && ( lmb[ 1 ] < 1.0 ) ) intersection[ 1 ] = vecStart[ 1 ] + vecDir[ 1 ] * lmb[ 1 ];
		else intersection[ 1 ] = vecStart[ 3 ] +  vecDir[ 3 ] * lmb[ 3 ];

		if      ( ( lmb[ 4 ] >= 0.0 ) && ( lmb[ 4 ] < 1.0 ) ) intersection[ 2 ] = vecStart[ 4 ] + vecDir[ 4 ] * lmb[ 4 ];
		else if ( ( lmb[ 5 ] >= 0.0 ) && ( lmb[ 5 ] < 1.0 ) ) intersection[ 2 ] = vecStart[ 5 ] + vecDir[ 5 ] * lmb[ 5 ];
		else intersection[ 2 ] = vecStart[ 7 ] + vecDir[ 7 ] * lmb[ 7 ];
		
		if	    ( ( lmb[ 6 ] >= 0.0 ) && ( lmb[ 6 ] < 1.0 ) ) intersection[ 3 ] = vecStart[ 6 ] + vecDir[ 6 ] * lmb[ 6 ];
		else if ( ( lmb[ 4 ] >= 0.0 ) && ( lmb[ 4 ] < 1.0 ) ) intersection[ 3 ] = vecStart[ 4 ] + vecDir[ 4 ] * lmb[ 4 ];
		else if ( ( lmb[ 5 ] >= 0.0 ) && ( lmb[ 5 ] < 1.0 ) ) intersection[ 3 ] = vecStart[ 5 ] + vecDir[ 5 ] * lmb[ 5 ];
		else intersection[ 3 ] = vecStart[ 7 ] + vecDir[ 7 ] * lmb[ 7 ];

		if	    ( ( lmb[ 8 ] >= 0.0 ) && ( lmb[ 8 ] < 1.0 ) ) intersection[ 4 ] = vecStart[ 8 ] + vecDir[ 8 ] * lmb[ 8 ];
		else if ( ( lmb[ 9 ] >= 0.0 ) && ( lmb[ 9 ] < 1.0 ) ) intersection[ 4 ] = vecStart[ 9 ] + vecDir[ 9 ] * lmb[ 9 ] ;
		else intersection[ 4 ] = vecStart[ 11 ]+ vecDir[ 11 ] * lmb[ 11 ];
		
		if	    ( ( lmb[ 10 ]>= 0.0 ) && ( lmb[ 10 ]< 1.0 ) ) intersection[ 5 ] = vecStart[ 10 ]+ vecDir[ 10 ] * lmb[ 10 ];
		else if ( ( lmb[ 8 ] >= 0.0 ) && ( lmb[ 8 ] < 1.0 ) ) intersection[ 5 ] = vecStart[ 8 ] + vecDir[ 8 ]  * lmb[ 8 ] ;
		else if ( ( lmb[ 9 ] >= 0.0 ) && ( lmb[ 9 ] < 1.0 ) ) intersection[ 5 ] = vecStart[ 9 ] + vecDir[ 9 ]  * lmb[ 9 ];
		else intersection[ 5 ] = vecStart[ 11 ]+ vecDir[ 11 ] * lmb[ 11 ];

# if 1
    ::glBegin ( GL_TRIANGLE_FAN );
   
    for( unsigned int k = 0; k < 6; ++k )
    {
#if 0
      float r ( 0.0 ), g ( 0.0 ), b ( 0.0 );
      float c ( 1.0 - ( float ( n ) / _numPlanes ) );
      Usul::Functions::hsvToRgb  ( r, g, b, c * 360.0f, 1.0f, 1.0f );
      
      ::glColor4f ( r, g, b, 1.0 );
#endif
      ::glVertex3fv ( intersection[k].ptr() );
    }

    ::glEnd();
#endif
    
    //state.setVertexPointer ( 3, GL_FLOAT, 0, &intersection.front() );
    //::glVertexPointer ( 3, GL_FLOAT, 0, &intersection.front() );
    //::glDrawElements ( GL_TRIANGLE_FAN, 6, GL_UNSIGNED_SHORT, &_indices.front() );
    
  }

  //state.disableVertexPointer ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Compute the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox PlanarProxyGeometry::computeBound() const
{
  return _bbox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void PlanarProxyGeometry::boundingBox ( const osg::BoundingBox& bb )
{
  _bbox = bb;
  this->_initCornersAndEdges ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

const osg::BoundingBox& PlanarProxyGeometry::boundingBox () const
{
  return _bbox;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of planes.
//
///////////////////////////////////////////////////////////////////////////////

void PlanarProxyGeometry::numPlanes ( unsigned int num )
{
  _numPlanes = num;
  _vertices.resize ( num );
  for ( std::vector < Vertices >::iterator iter = _vertices.begin(); iter != _vertices.end(); ++iter )
    iter->resize ( 6 );
}
