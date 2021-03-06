
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgSquare.cpp: Square class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgSquare.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgSquare,SgPrimitive);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSquare::SgSquare() : SgPrimitive(),
  center ( 0.0f, 0.0f, 0.0f ),
  size ( 1.0f )
{
  SL_PRINT2 ( "SgSquare::SgSquare(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSquare::SgSquare ( const SlVec3f &c, const SlFloat32 &s ) : SgPrimitive(),
  center ( c ),
  size ( s )
{
  SL_PRINT2 ( "SgSquare::SgSquare(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgSquare::~SgSquare()
{
  SL_PRINT2 ( "SgSquare::SgSquare(), this = %X\n", this );
}
