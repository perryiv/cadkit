
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgMaterial, SgAttribute );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgMaterial::SgMaterial() : SgAttribute(), 
  side ( FRONT ),
  material ( SlVec4f ( SG_DEFAULT_MATERIAL_AMBIENT ), 
             SlVec4f ( SG_DEFAULT_MATERIAL_DIFFUSE ), 
             SlVec4f ( SG_DEFAULT_MATERIAL_SPECULAR ), 
             SlVec4f ( SG_DEFAULT_MATERIAL_EMISSIVE ), 
             SG_DEFAULT_MATERIAL_SHININESS )
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
  material ( copyMe.material )
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

void SgMaterial::setValue ( const SgMaterial &m )
{
  SL_ASSERT ( this );
  material.setValue ( m.material );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Does the given node have the same visible properties?
//
/////////////////////////////////////////////////////////////////////////////

bool SgMaterial::isEqualVisibleProperties ( const SgNode &node ) const
{
  SL_ASSERT ( this );

  // Make sure we have the right type.
  if ( false == node.isOfType ( SgMaterial::getClassType() ) )
    return false;

  // Typecast.
  SgMaterial &c = (SgMaterial &) node;

  // Are they the same?
  return ( 
    material.isEqual ( c.material ) && 
    SgAttribute::isEqualVisibleProperties ( node ) );
}
