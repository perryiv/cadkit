
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDrawStyle.cpp: Drawing style class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgDrawStyle.h"

#include "Standard/SlPrint.h"
#include "Standard/SlAssert.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgDrawStyle, SgAttribute );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgDrawStyle::SgDrawStyle() : SgAttribute(),
  polygonStyle ( FILLED ),
  side ( FRONT_AND_BACK ),
  pointSize ( 1.0f ),
  lineWidth ( 1.0f ),
  lineStippleFactor ( 1 ),
  lineStipplePattern ( 0xffff )
{
  SL_PRINT2 ( "SgDrawStyle::SgDrawStyle(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgDrawStyle::~SgDrawStyle()
{
  SL_PRINT2 ( "SgDrawStyle::~SgDrawStyle(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
/////////////////////////////////////////////////////////////////////////////

bool SgDrawStyle::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // Make sure we have the right type.
  if ( false == node.isOfType ( SgDrawStyle::getClassType() ) )
    return false;

  // Typecast.
  SgDrawStyle &d = (SgDrawStyle &) node;

  // Are they the same?
  return ( 
    polygonStyle       == d.polygonStyle && 
    side               == d.side && 
    pointSize          == d.pointSize && 
    lineWidth          == d.lineWidth && 
    lineStippleFactor  == d.lineStippleFactor && 
    lineStipplePattern == d.lineStipplePattern && 
    SgAttribute::isEqualVisibleProperties ( node ) );
}
