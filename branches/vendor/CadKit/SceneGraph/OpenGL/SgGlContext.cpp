
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
//  SgGlContext: The rendering context class.
//
///////////////////////////////////////////////////////////////////////////////

#include "SgGlPrecompiled.h"
#include "SgGlContext.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlAssert.h"
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SL_IMPLEMENT_CLASS(SgGlContext,SlRefBase);


///////////////////////////////////////////////////////////////////////////////
//
// If this was a reference pointer then it would be unreferenced (and deleted) 
// too late in the game.
//
///////////////////////////////////////////////////////////////////////////////

SgGlContext *SgGlContext::_current = NULL;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlContext::SgGlContext() : SlRefBase ( 0 )
{
  SL_PRINT2 ( "SgGlContext::SgGlContext(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

SgGlContext::~SgGlContext()
{
  SL_PRINT2 ( "SgGlContext::~SgGlContext(), this = %X\n", this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make this context current.
//
///////////////////////////////////////////////////////////////////////////////

bool SgGlContext::makeCurrent()
{
  SL_ASSERT ( this );
  _current = this;
  return true;
}
