
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
//  SgMaterial.cpp: Material class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgMaterial.h"
#include "SgDefine.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlPrint.h"
#endif

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE(SgMaterial,SgAttribute);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMaterial::SgMaterial() : SgAttribute(), 
  side ( FRONT ),
  ambient   ( SG_DEFAULT_MATERIAL_AMBIENT), 
  diffuse   ( SG_DEFAULT_MATERIAL_DIFFUSE ), 
  specular  ( SG_DEFAULT_MATERIAL_SPECULAR ), 
  emissive  ( SG_DEFAULT_MATERIAL_EMISSIVE ), 
  shininess ( SG_DEFAULT_MATERIAL_SHININESS )
{
  SL_PRINT2 ( "SgMaterial::SgMaterial(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMaterial::SgMaterial ( const SgMaterial &copyMe ) : SgAttribute(), 
  side ( copyMe.side ),
  ambient ( copyMe.ambient ), 
  diffuse ( copyMe.diffuse ), 
  specular ( copyMe.specular ), 
  emissive ( copyMe.emissive ), 
  shininess ( copyMe.shininess )
{
  SL_PRINT2 ( "SgMaterial::SgMaterial(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMaterial::~SgMaterial()
{
  SL_PRINT2 ( "SgMaterial::~SgMaterial(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the material value.
//
/////////////////////////////////////////////////////////////////////////////

void SgMaterial::setValue ( const SgMaterial &material )
{
  SL_ASSERT ( this );
  ambient.setValue ( material.ambient );
  diffuse.setValue ( material.diffuse );
  specular.setValue ( material.specular );
  emissive.setValue ( material.emissive );
  shininess = material.shininess;
}
