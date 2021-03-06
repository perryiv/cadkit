
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SgCoordinate3.cpp: 3D coordinates.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgCoordinate3.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgCoordinate3, SgCoordinate );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgCoordinate3::SgCoordinate3() : SgCoordinate()
{
  SL_PRINT2 ( "SgCoordinate3::SgCoordinate3(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgCoordinate3::~SgCoordinate3()
{
  SL_PRINT2 ( "SgCoordinate3::~SgCoordinate3(), this = %X\n", this );
  _points.clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
/////////////////////////////////////////////////////////////////////////////

bool SgCoordinate3::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // Make sure we have the right type.
  if ( false == node.isOfType ( SgCoordinate3::getClassType() ) )
    return false;

  // Typecast.
  SgCoordinate3 &c = (SgCoordinate3 &) node;

  // Are they the same?
  return ( _points.isEqual ( c._points ) && SgCoordinate::isEqualVisibleProperties ( node ) );
}
