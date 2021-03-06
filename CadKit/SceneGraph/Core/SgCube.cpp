
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCube.cpp: Cube class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgCube.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgCube,SgPrimitive);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCube::SgCube() : SgPrimitive(),
  _center ( 0.0, 0.0, 0.0 ),
  _size ( 1.0f )
{
  SL_PRINT2 ( "SgCube::SgCube(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCube::SgCube ( const SlVec3f &center, const SlFloat32 &size ) : SgPrimitive(),
  _center ( center ),
  _size ( size )
{
  SL_PRINT2 ( "SgCube::SgCube(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCube::SgCube ( const SgCube &cube ) : SgPrimitive(),
  _center ( cube._center ),
  _size ( cube._size )
{
  SL_PRINT2 ( "SgCube::SgCube(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCube::~SgCube()
{
  SL_PRINT2 ( "SgCube::~SgCube(), this = %X\n", this );
}
