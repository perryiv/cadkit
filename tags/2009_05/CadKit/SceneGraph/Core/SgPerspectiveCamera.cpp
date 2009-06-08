
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgPerspectiveCamera.h: Perspective camera class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgPerspectiveCamera.h"
#include "SgGetBoundingBox.h"
#include "SgDefine.h"

#include "Standard/SlLine2.h"
#include "Standard/SlPrint.h"
#include "Standard/SlConstants.h"
#include "Standard/SlMinMax.h"

using namespace CadKit;

SG_IMPLEMENT_DYNAMIC_NODE ( SgPerspectiveCamera, SgCamera );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgPerspectiveCamera::SgPerspectiveCamera() : SgCamera(), 
  fieldOfViewOrientation ( FOV_IN_X ),
  _fieldOfView ( SG_DEFAULT_PERSPECTIVE_CAMERA_HEIGHT )
{
  SL_PRINT2 ( "SgPerspectiveCamera::SgPerspectiveCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgPerspectiveCamera::SgPerspectiveCamera ( const SgPerspectiveCamera &camera ) : 
  SgCamera ( camera ), 
  fieldOfViewOrientation ( camera.fieldOfViewOrientation ),
  _fieldOfView ( camera._fieldOfView )
{
  SL_PRINT2 ( "SgPerspectiveCamera::SgPerspectiveCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgPerspectiveCamera::~SgPerspectiveCamera()
{
  SL_PRINT2 ( "SgPerspectiveCamera::~SgPerspectiveCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Move the camera so that it can see the entire scene. 
//
//  Note 1: if you give this function a sub-branch of a parent scene that 
//  has transformations in it, then this function will not give the expected 
//  results. There is no way to determine what those transformations are.
//
//  Note 2: I hard coded in the center of the camera at (0.0, 0.0, -_distance).
//  I tried getting the bounding box of the scene twice, once before 
//  setCenter() and once after. This didn't work well if there were rotations
//  and/or scale nodes in the scene. Not sure why. The problem went away when
//  I just used the global space bounding box.
//
//  Note 3: I don't check for fieldOfViewOrientation. I always use the minumum
//  scale according to what will make it fit in the current window. I figured
//  a viewAll() function should make everything visible every time.
//
/////////////////////////////////////////////////////////////////////////////

bool SgPerspectiveCamera::viewAll ( SgNode &scene ) 
{
  SL_ASSERT ( this ) ;

  // Get the bounding box of the scene in global space. This is because the 
  // SgGetBoundingBox's matrix starts out as identity.
  SgGetBoundingBox visitor;
  if ( false == scene.accept ( visitor ) ) 
    return false;
  const SlBoundingBoxf globalBBox = visitor.getBBox();

  // Set the center. This will change the camera's space (with respect 
  // to the global space).
  if ( !this->setCenter ( globalBBox.getCenter(), true ) ) 
    return false;

  // Get the bounding sphere in global space.
  SlVec3f globalSphereCenter;
  float globalSphereRadius;
  globalBBox.getBoundingSphere ( globalSphereCenter, globalSphereRadius );

  // This line starts at the viewpoint origin (the vertex of the 
  // frustum) and goes out along the right frustum wall.
  float alphaRight = SL_DEG_TO_RAD_F * _fieldOfView * 0.5f;
  SlLine2f rightFrustumWall ( SlVec2f ( 0.0f, 0.0f ), SlVec2f ( ::sinf ( alphaRight ), -::cosf ( alphaRight ) ) );

  // Get the distance from the bounding sphere's center to the right 
  // frustum wall.
  SlVec2f cameraCenter ( 0.0f, -_distance );
  float radiusToRightFrustumWall = rightFrustumWall.getDistance ( cameraCenter );

  // This line starts at the viewpoint origin (the vertex of the 
  // frustum) and goes out along the top frustum wall.
  float alphaTop = alphaRight / _aspect;
  SlLine2f topFrustumWall ( SlVec2f ( 0.0f, 0.0f ), SlVec2f ( ::sinf ( alphaTop ), -::cosf ( alphaTop ) ) );

  // Get the distance from the bounding sphere's center to the top 
  // frustum wall.
  float radiusToTopFrustumWall = topFrustumWall.getDistance ( cameraCenter );

  // The scale factor is the number that will make the the two radii equal.
  // Use the minimum radius to a frustum wall.
  float s = _viewAllFactor * ( std::min ( radiusToRightFrustumWall, radiusToTopFrustumWall ) ) / globalSphereRadius;
  this->setScale ( s );

  // It worked!
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

SlMatrix44f SgPerspectiveCamera::getProjectionMatrix() const
{
  SL_ASSERT ( this );
  SlMatrix44f P;
  this->getProjectionMatrix ( P );
  return P;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix as:
//
//  |    f                                   |
//  |  ------  0      0            0         |
//  |  aspect                                |
//  |                                        |
//  |    0     f      0            0         |
//  |                                        |
//  |             zFar+zNear  2*zFar*zNear   |
//  |    0     0  ----------  ------------   |
//  |             zNear-zFar   zNear-zFar    |
//  |                                        |
//  |    0     0      -1           0         |
//
//  Where f = cotangent(fovy/2).
//
/////////////////////////////////////////////////////////////////////////////

void SgPerspectiveCamera::getProjectionMatrix ( SlMatrix44f &P ) const
{
  SL_ASSERT ( this );

  float f = 1.0f / ( ::tanf ( this->getFieldOfView() * 0.5f ) );
  float invZDiff = 1.0f / ( _zNear - _zFar );

  P[0]  = f / _aspect;
  P[1]  = 0.0f;
  P[2]  = 0.0f;
  P[3]  = 0.0f;

  P[4]  = 0.0f;
  P[5]  = f;
  P[6]  = 0.0f;
  P[7]  = 0.0f;

  P[8]  = 0.0f;
  P[9]  = 0.0f;
  P[10] = ( _zFar + _zNear ) * invZDiff;
  P[11] = -1.0f;

  P[12] = 0.0f;
  P[13] = 0.0f;
  P[14] = 2.0f * _zFar * _zNear * invZDiff;
  P[15] = 0.0f;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the proper field of view for the projection, based on 
//  FieldOfViewOrientation and _fieldOfView.
//
/////////////////////////////////////////////////////////////////////////////

SlFloat32 SgPerspectiveCamera::getFieldOfView ( const FieldOfViewOrientation &flag ) const
{
  SL_ASSERT ( this );

  // See how we are supposed to treat the field of view. If the field of 
  // view is supposed to be in the x-direction then we have to multiply it 
  // by the aspect ratio because gluPerspective() wants it in the y-direction. 
  // Using the min-direction keeps the geometry always visible (assuming it 
  // already is) when you resize the window.
  switch ( flag )
  {
  case SgPerspectiveCamera::FOV_IN_X:    return _fieldOfView / _aspect;
  case SgPerspectiveCamera::FOV_IN_Y:    return _fieldOfView;
  case SgPerspectiveCamera::FOV_IN_MIN:  return ( _aspect >= 1.0f ) ? _fieldOfView : _fieldOfView / _aspect;
  case SgPerspectiveCamera::FOV_IN_MAX:  return ( _aspect <= 1.0f ) ? _fieldOfView : _fieldOfView / _aspect;
  default: SL_ASSERT ( 0 ); return _fieldOfView; // You goofed.
  }
}
