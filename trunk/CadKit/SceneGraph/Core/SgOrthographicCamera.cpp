
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgOrthographicCamera.h: Orthographic camera class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgOrthographicCamera.h"
#include "SgDefine.h"

#include "Standard/SlPrint.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgOrthographicCamera, SgCamera );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgOrthographicCamera::SgOrthographicCamera() : SgCamera(), 
  _height ( SG_DEFAULT_ORTHOGRAPHIC_CAMERA_HEIGHT )
{
  SL_PRINT2 ( "SgOrthographicCamera::SgOrthographicCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgOrthographicCamera::SgOrthographicCamera ( const SgOrthographicCamera &camera ) : 
  SgCamera ( camera ), 
  _height ( camera._height )
{
  SL_PRINT2 ( "SgOrthographicCamera::SgOrthographicCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgOrthographicCamera::~SgOrthographicCamera()
{
  SL_PRINT2 ( "SgOrthographicCamera::~SgOrthographicCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

SlMatrix44f SgOrthographicCamera::getProjectionMatrix() const
{
  SL_ASSERT ( this );

  SlMatrix44f P;
  this->getProjectionMatrix ( P );
  return P;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

void SgOrthographicCamera::getProjectionMatrix ( SlMatrix44f &P ) const
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); // TODO.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Move the camera so that it can see the entire scene.
//
/////////////////////////////////////////////////////////////////////////////

bool SgOrthographicCamera::viewAll ( SgNode &scene ) 
{
  SL_ASSERT ( this );
  SL_ASSERT ( 0 ); // TODO.
  return false;
}
