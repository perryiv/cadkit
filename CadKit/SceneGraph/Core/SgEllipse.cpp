
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
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
# include "Standard/SlString.h"
# include "Standard/SlFunctions.h"
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
  SL_PRINT ( "SgEllipse::SgEllipse(), this = %X\n", this );
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
  SL_PRINT ( "SgEllipse::SgEllipse(), this = %X\n", this );
  SL_ASSERT ( numVerts > 1 );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgEllipse::~SgEllipse()
{
  SL_PRINT ( "SgEllipse::~SgEllipse(), this = %X\n", this );
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
