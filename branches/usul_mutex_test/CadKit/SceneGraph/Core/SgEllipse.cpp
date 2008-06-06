
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgEllipse:
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgEllipse.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
# include "Standard/SlConstants.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgEllipse,SgPrimitive);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgEllipse::SgEllipse() : SgPrimitive(), 
  _center ( 0.0f, 0.0f, 0.0f ),
  _majorRadius ( 1.0f ), 
  _minorRadius ( 0.5f ), 
  _numVerts ( 21 ),
  _needToMakeNewVertices ( true ),
  _vertex ( 0x0 )
{
  SL_PRINT2 ( "SgEllipse::SgEllipse(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgEllipse::SgEllipse (  const SlVec3f &center, 
            const SlFloat32 &majorRadius, 
            const SlFloat32 &minorRadius, 
            const SlInt32 &numVerts ) : SgPrimitive(), 
  _center ( center ), 
  _majorRadius ( majorRadius ), 
  _minorRadius ( minorRadius ), 
  _numVerts ( numVerts ),
  _needToMakeNewVertices ( true ),
  _vertex ( 0x0 )
{
  SL_PRINT2 ( "SgEllipse::SgEllipse(), this = %X\n", this );
  SL_ASSERT ( numVerts > 1 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgEllipse::~SgEllipse()
{
  SL_PRINT2 ( "SgEllipse::~SgEllipse(), this = %X\n", this );
  if ( _vertex ) delete [] _vertex;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Make the vertices.
//
/////////////////////////////////////////////////////////////////////////////

bool SgEllipse::makeVertices()
{
  SL_ASSERT ( this );
  SL_ASSERT ( _numVerts > 1 );

  // Allocate the vertices.
  if ( _vertex ) delete [] _vertex;
  _vertex = new SlVec3f[_numVerts];

  // Check allocation.
  if ( NULL == _vertex ) 
  {
    SL_ASSERT ( 0 );
    return false;
  }

  // For loop speed.
  SlFloat32 denom = SlFloat32 ( _numVerts - 1 );
  SlFloat32 invDenom = 1.0f / denom, u, q, r;

  for ( SlInt32 i = 0; i < _numVerts; ++i )
  {
    u = SlFloat32 ( i );
    q = u * invDenom;
    r = u - q * denom;
    u = q + r * invDenom;
    u = u * SL_2_PI_F;

    // Evaluate the parametric equation of an ellipse.
    _vertex[i].setValue ( _center[0] + _majorRadius * cosf ( u ), _center[1] + _minorRadius * sinf ( u ), _center[2] );
  }

  _needToMakeNewVertices = false;
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setCenter ( const SlVec3f &center )
{
  SL_ASSERT ( this );
  _center.setValue ( center );
  _needToMakeNewVertices = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the center.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setCenter ( const SlFloat32 &x, const SlFloat32 &y, const SlFloat32 &z )
{
  SL_ASSERT ( this );
  _center.setValue ( x, y, z );
  _needToMakeNewVertices = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the radius.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setMajorRadius ( const SlFloat32 &majorRadius )
{
  SL_ASSERT ( this );
  _majorRadius = majorRadius;
  _needToMakeNewVertices = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the radius.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setMinorRadius ( const SlFloat32 &minorRadius )
{
  SL_ASSERT ( this );
  _minorRadius = minorRadius;
  _needToMakeNewVertices = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the number of vertices.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setNumVertices ( const SlInt32 &numVerts )
{
  SL_ASSERT ( this && numVerts > 1 );
  _numVerts = numVerts;
  _needToMakeNewVertices = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the radius.
//
/////////////////////////////////////////////////////////////////////////////

void SgEllipse::setRadii ( const SlFloat32 &majorRadius, const SlFloat32 &minorRadius )
{
  SL_ASSERT ( this );
  _majorRadius = majorRadius;
  _minorRadius = minorRadius;
  _needToMakeNewVertices = true;
}
