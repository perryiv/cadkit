
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
//  SgSphere.cpp: Sphere class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgSphere.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgSphere,SgPrimitive);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSphere::SgSphere() : SgPrimitive(),
  _tessellationType ( NO_POLES ),
  _center ( 0.0f, 0.0f, 0.0f ),
  _radius ( 1.0f ),
  _member0 ( 2 ), 
  _member1 ( 1 )
{
  SL_PRINT2 ( "SgSphere::SgSphere(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSphere::SgSphere ( const SgSphere &copyMe ) : SgPrimitive(),
  _tessellationType ( copyMe._tessellationType ),
  _center ( copyMe._center ),
  _radius ( copyMe._radius ),
  _member0 ( copyMe._member0 ), 
  _member1 ( copyMe._member1 )
{
  SL_PRINT2 ( "SgSphere::SgSphere(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSphere::~SgSphere()
{
  SL_PRINT2 ( "SgSphere::SgSphere(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the number of slices.
//
/////////////////////////////////////////////////////////////////////////////

void SgSphere::setNumSlices ( const SlUint32 &numSlices )
{
  SL_ASSERT ( this && numSlices > 1 );
  SL_ASSERT ( POLES == _tessellationType );
  _member0 = numSlices;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the number of stacks.
//
/////////////////////////////////////////////////////////////////////////////

void SgSphere::setNumStacks ( const SlUint32 &numStacks )
{
  SL_ASSERT ( this && numStacks > 1 );
  SL_ASSERT ( POLES == _tessellationType );
  _member1 = numStacks;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the number of subdivisions.
//
/////////////////////////////////////////////////////////////////////////////

void SgSphere::setNumSubdivisions ( const SlUint32 &numSubdivisions )
{
  SL_ASSERT ( this );
  SL_ASSERT ( NO_POLES == _tessellationType );
  _member1 = numSubdivisions;
}
