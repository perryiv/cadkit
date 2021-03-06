
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgRotation.cpp: A Rotation class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgRotation.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgRotation, SgTransformation );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgRotation::SgRotation() : SgTransformation(), 
  _axis ( 0.0f, 0.0f, 1.0f ),
  _degrees ( 0.0f )
{
  SL_PRINT2 ( "SgRotation::SgRotation(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgRotation::SgRotation ( const SgRotation &rotation ) : SgTransformation ( rotation ),
  _axis ( rotation._axis ),
  _degrees ( rotation._degrees )
{
  SL_PRINT2 ( "SgRotation::SgRotation(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgRotation::SgRotation ( const SlVec3f &axis, const float &degrees ) : SgTransformation(),
  _axis ( axis ),
  _degrees ( degrees )
{
  SL_PRINT2 ( "SgRotation::SgRotation(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgRotation::~SgRotation()
{
  SL_PRINT2 ( "SgRotation::~SgRotation(), this = %X\n", this );
}
