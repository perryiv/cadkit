
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCircle.cpp: Sphere class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgCircle.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgCircle,SgPrimitive);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCircle::SgCircle() : SgPrimitive(),
  center ( 0.0, 0.0, 0.0 ),
  radius ( 1.0 )
{
  SL_PRINT2 ( "SgCircle::SgCircle(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCircle::SgCircle ( const SlVec3f &c, const SlFloat32 &r ) : SgPrimitive(),
  center ( c ),
  radius ( r )
{
  SL_PRINT2 ( "SgCircle::SgCircle(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCircle::~SgCircle()
{
  SL_PRINT2 ( "SgCircle::SgCircle(), this = %X\n", this );
}