
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgTransform.cpp: A Transformation class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgTransform.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgTransform, SgTransformation );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransform::SgTransform() : SgTransformation(), _matrix ( SL_MATRIX_44_IDENTITY_F )
{
  SL_PRINT2 ( "SgTransform::SgTransform(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransform::SgTransform ( const SlMatrix44f &matrix ) : SgTransformation(), _matrix ( matrix )
{
  SL_PRINT2 ( "SgTransform::SgTransform(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransform::SgTransform ( const SgTransform &t ) : SgTransformation ( t ), _matrix ( t._matrix )
{
  SL_PRINT2 ( "SgTransform::SgTransform(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgTransform::~SgTransform()
{
  SL_PRINT2 ( "SgTransform::~SgTransform(), this = %X\n", this );
}
