
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
//  SgBackground.cpp: Background class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgBackground.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgBackground,SgAttribute);


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
  SL_PRINT ( "SgBackground::SgBackground(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgBackground::SgBackground ( const SgBackground &background ) : SgAttribute ( background )
{
  SL_PRINT ( "SgBackground::SgBackground(), this = %X\n", this );
  this->setValue ( background );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgBackground::~SgBackground()
{
  SL_PRINT ( "SgBackground::~SgBackground(), this = %X\n", this );
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
