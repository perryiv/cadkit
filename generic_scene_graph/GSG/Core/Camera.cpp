
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Like a Transform but modifies the projection too.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Camera.h"
#include "GSG/Core/Visitor.h"
#include "GSG/Core/PushPop.h"
#include "GSG/Core/Math.h"

using namespace GSG;

GSG_IMPLEMENT_ACCEPT_NODE ( Camera );
GSG_IMPLEMENT_CLONE  ( Camera );


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Camera::Camera() : Group(),
  _valid ( false ),
  _projection (),
  _modelview  (),
  _fov        ( GSG::PIE / 4 ),
  _aspect     ( 1 ),
  _nearZ      ( 0.01f ),
  _farZ       ( 1e6f ),
  _scale      ( 1 ),
  _origin     ( 0, 0, 0 ),
  _center     ( 0, 0, 0 ),
  _distance   ( 1 ),
  _rotation   (),
  _vaFactor   ( 1 )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

Camera::Camera ( const Camera &c ) : Group ( c ),
  _valid      ( c._valid ),
  _projection ( c._projection ),
  _modelview  ( c._modelview ),
  _fov        ( c._fov ),
  _aspect     ( c._aspect ),
  _nearZ      ( c._nearZ ),
  _farZ       ( c._farZ ),
  _scale      ( c._scale ),
  _origin     ( c._origin ),
  _center     ( c._center ),
  _distance   ( c._distance ),
  _rotation   ( c._rotation ),
  _vaFactor   ( c._vaFactor )
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

Camera::~Camera()
{
  // Empty.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Change the view so that all of the children are visible.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::viewAll()
{
  Lock lock ( this );
  // TODO
//  See how OSG does this.
//  You may not need all of these members.
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the aspect ratio.
//
/////////////////////////////////////////////////////////////////////////////

Real Camera::aspectRatio() const
{
  Lock lock ( this );
  return _aspect;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the aspect ratio.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::aspectRatio ( UnsignedInteger w, UnsignedInteger h )
{
  this->aspectRatio ( ( static_cast<Real>(w) ) / ( static_cast<Real>(h) ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the aspect ratio.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::aspectRatio ( Real aspect )
{
  Lock lock ( this );
  _aspect = aspect;
  _valid = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the field of view.
//
/////////////////////////////////////////////////////////////////////////////

Real Camera::fieldOfView() const
{
  Lock lock ( this );
  return _fov;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the field of view.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::fieldOfView ( Real fov )
{
  Lock lock ( this );
  _fov = fov;
  _valid = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the z-distance to the near plane.
//
/////////////////////////////////////////////////////////////////////////////

Real Camera::nearZ() const
{
  Lock lock ( this );
  return _nearZ;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the z-distance to the near plane.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::nearZ ( Real n )
{
  Lock lock ( this );
  _nearZ = n;
  _valid = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the z-distance to the far plane.
//
/////////////////////////////////////////////////////////////////////////////

Real Camera::farZ() const
{
  Lock lock ( this );
  return _farZ;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the z-distance to the far plane.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::farZ ( Real f )
{
  Lock lock ( this );
  _farZ = f;
  _valid = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Modify the rotation.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::rotate ( const Matrix &R )
{
  Lock lock ( this );

  // Rotate the origin and the current rotation.
  _origin = R * _origin;
  _rotation = R * _rotation;

  // Mark as invalid.
  _valid = false;
}


/////////////////////////////////////////////////////////////////////////////
//
//  This does trackball transformations.
//
/////////////////////////////////////////////////////////////////////////////

void Camera::_updateMatrices()
{
  Lock lock ( this );
  ErrorChecker ( !_valid );

  //
  // Modelview
  //

  // Put the camera in the absolute global space with no scaling.
  _modelview.identity();

  // Note: when you reason this out, from here down you have to read 
  // bottom up. In other words, you first translate to the center of 
  // the world, then you rotate, ...

  // Back it up so we can see it.
  _modelview.translate ( Vec3 ( 0, 0, -_distance ) );

  // Scale the world.
  _modelview.scale ( Vec3 ( _scale, _scale, _scale ) );

  // Translation offset.
  _modelview.translate ( Vec3 ( _origin[0], _origin[1], _origin[2] ) );

  // Rotate the world.
  _modelview.postMultiply ( _rotation );

  // Move to the center of rotaion.
  _modelview.translate ( Vec3 ( -_center[0], -_center[1], -_center[2] ) );

  //
  // Projection
  //

  // Set the projection.
  _projection.perspective ( GSG::Math::degToRad ( _fov ), _aspect, _nearZ, _farZ );

  // The matrices are now valid.
  _valid = true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the modelview matrix.
//
/////////////////////////////////////////////////////////////////////////////

const Matrix &Camera::modelviewMatrix() const
{
  Lock lock ( this );

  // Update it if we need to.
  if ( !_valid )
  {
    // Doing the const-cast because I want this function to remain const.
    // The lazy matrix update is an implementation detail that should not 
    // effect the interface.
    Camera *me = const_cast < Camera * > ( this );
    me->_updateMatrices();
  }

  // Return the matrix.
  return _modelview;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Get the projection matrix.
//
/////////////////////////////////////////////////////////////////////////////

const Matrix &Camera::projectionMatrix() const
{
  Lock lock ( this );

  // Update it if we need to.
  if ( !_valid )
  {
    // Doing the const-cast because I want this function to remain const.
    // The lazy matrix update is an implementation detail that should not 
    // effect the interface.
    Camera *me = const_cast < Camera * > ( this );
    me->_updateMatrices();
  }

  // Return the matrix.
  return _projection;
}
