
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgDirectionalLight.cpp: Directional light class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgDirectionalLight.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgDirectionalLight, SgLight );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgDirectionalLight::SgDirectionalLight() : SgLight(), 
  _direction ( 0.0f, 0.0f, -1.0f )
{
  SL_PRINT2 ( "SgDirectionalLight::SgDirectionalLight(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgDirectionalLight::SgDirectionalLight ( const SgDirectionalLight &light ) : SgLight ( light ), 
  _direction ( 0.0f, 0.0f, -1.0f )
{
  SL_PRINT2 ( "SgDirectionalLight::SgDirectionalLight(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgDirectionalLight::~SgDirectionalLight()
{
  SL_PRINT2 ( "SgDirectionalLight::~SgDirectionalLight(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the light value.
//
/////////////////////////////////////////////////////////////////////////////

void SgDirectionalLight::setValue ( const SgDirectionalLight &light )
{
  _direction.setValue ( light._direction );

  // Call the base class's function.
  SgLight::setValue ( light );
}