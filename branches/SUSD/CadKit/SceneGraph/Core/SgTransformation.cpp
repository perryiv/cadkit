
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTransformation.cpp: Abstract base transformation class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgTransformation.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_NODE ( SgTransformation, SgNode );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransformation::SgTransformation() : SgNode()
{
  SL_PRINT2 ( "SgTransformation::SgTransformation(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransformation::SgTransformation ( const SgTransformation &t ) : SgNode ( t )
{
  SL_PRINT2 ( "SgTransformation::SgTransformation(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransformation::~SgTransformation()
{
  SL_PRINT2 ( "SgTransformation::~SgTransformation(), this = %X\n", this );
}