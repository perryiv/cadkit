
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgBackground.cpp: Background class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgBackground.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgBackground, SgAttribute );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgBackground::SgBackground() : SgAttribute(), 
  _topLeftCorner     ( 0.18f, 0.44f, 0.85f, 1.0f ), // Pretty color?
  _topRightCorner    ( 0.18f, 0.44f, 0.85f, 1.0f ),
  _bottomLeftCorner  ( 0.06f, 0.06f, 0.30f, 1.0f ),
  _bottomRightCorner ( 0.06f, 0.06f, 0.30f, 1.0f ),
  _farDistance       ( 59000.0f ) // Why this number?
{
  SL_PRINT2 ( "SgBackground::SgBackground(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgBackground::SgBackground ( const SgBackground &background ) : SgAttribute ( background )
{
  SL_PRINT2 ( "SgBackground::SgBackground(), this = %X\n", this );
  this->setValue ( background );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgBackground::~SgBackground()
{
  SL_PRINT2 ( "SgBackground::~SgBackground(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the material value.
//
/////////////////////////////////////////////////////////////////////////////

void SgBackground::setValue ( const SgBackground &background )
{
  this->setTopLeftCorner     ( background._topLeftCorner );
  this->setTopRightCorner    ( background._topRightCorner );
  this->setBottomLeftCorner  ( background._bottomLeftCorner );
  this->setBottomRightCorner ( background._bottomRightCorner );
  this->setFarDistance       ( background._farDistance );
}
