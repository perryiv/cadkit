
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

///////////////////////////////////////////////////////////////////////////////
//
//  SgCoordinate4.cpp: 4D coordinates.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgCoordinate4.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgCoordinate4,SgCoordinate);


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgCoordinate4::SgCoordinate4() : SgCoordinate()
{
  SL_PRINT2 ( "SgCoordinate4::SgCoordinate4(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgCoordinate4::~SgCoordinate4()
{
  SL_PRINT2 ( "SgCoordinate4::~SgCoordinate4(), this = %X\n", this );
  _points.clear();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
/////////////////////////////////////////////////////////////////////////////

bool SgCoordinate4::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // Make sure we have the right type.
  if ( false == node.isOfType ( SgCoordinate4::getClassType() ) )
    return false;

  // Typecast.
  SgCoordinate4 &c = (SgCoordinate4 &) node;

  // Are they the same?
  return ( _points.isEqual ( c._points ) && SgCoordinate::isEqualVisibleProperties ( node ) );
}
