
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
//  SgCamera.h: Base camera class.
//
/////////////////////////////////////////////////////////////////////////////

#include "SgPrecompiled.h"
#include "SgCamera.h"
#include "SgDefine.h"

using namespace CadKit;

SG_IMPLEMENT_NODE(SgCamera,SgNode);


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCamera::SgCamera() : SgNode(),
  _mode ( MODELVIEW ),
  _aspect ( SG_DEFAULT_CAMERA_ASPECT_RATIO ),
  _zNear ( SG_DEFAULT_CAMERA_Z_NEAR ),
  _zFar ( SG_DEFAULT_CAMERA_Z_FAR ),
  _scale ( SG_DEFAULT_CAMERA_SCALE ),
  _origin ( 0.0f, 0.0f, 0.0f ),
  _center ( 0.0f, 0.0f, 0.0f ),
  _distance ( 1.0f ), // See note below.
  _R ( SL_MATRIX4_IDENTITY_F ),
  _M ( SL_MATRIX4_IDENTITY_F ),
  _viewAllFactor ( SG_DEFAULT_CAMERA_VIEW_ALL_FACTOR )
{
  SL_PRINT ( "SgCamera::SgCamera(), this = %X\n", this );

  // Note: the default _distance is not zero for a reason. If we make it 
  // zero then when the client calls viewAll(), with the camera's _distance 
  // being 0, the calculated scale is always 0. This is bad.

  // We do this because the distance and the matrix should be consistant.
  _M[14] = -_distance;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCamera::SgCamera ( const SgCamera &camera ) : SgNode ( camera ),
  _mode ( camera._mode ),
  _aspect ( camera._aspect ),
  _zNear ( camera._zNear ),
  _zFar ( camera._zFar ),
  _scale ( camera._scale ),
  _origin ( camera._origin ),
  _center ( camera._center ),
  _distance ( camera._distance ),
  _R ( camera._R ),
  _M ( camera._M ),
  _viewAllFactor ( camera._viewAllFactor )
{
  SL_PRINT ( "SgCamera::SgCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

SgCamera::~SgCamera()
{
  SL_PRINT ( "SgCamera::~SgCamera(), this = %X\n", this );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the value of the node.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::setValue ( const SgCamera &camera )
{
  SL_ASSERT ( this );

  // Call the base class's function.
  this->SgNode::setValue ( camera );

  // Set this class's data members.
  _mode = camera._mode;
  _aspect = camera._aspect;
  _zNear = camera._zNear;
  _zFar = camera._zFar;
  _scale = camera._scale;
  _origin = camera._origin;
  _center = camera._center;
  _distance = camera._distance;
  _R.setValue ( camera._R );
  _M.setValue ( camera._M );
  _viewAllFactor = camera._viewAllFactor;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Modify the rotation.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::rotate ( const SlMatrix4f &R )
{
  SL_ASSERT ( this );

  // Rotate the origin and the current rotation.
  _origin = R * _origin;
  _R = R * _R;

  // Calculate the modelview.
  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation.
//
/////////////////////////////////////////////////////////////////////////////

bool SgCamera::setRotation ( const SlMatrix4f &R )
{
  SL_ASSERT ( this );

  // Get the inverse of the current rotation.
  SlMatrix4f IR = _R;
  if ( !IR.invert() ) 
    return false;

  // Put the origin back.
  _origin = IR * _origin;

  // Assign new rotation.
  _R = R;

  // Place the origin.
  _origin = R * _origin;

  // Calculate the modelview.
  this->_matrixCalc();

  // It worked.
  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Modify the translation.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::translate ( const float &x, const float &y, const float &z )
{
  SL_ASSERT ( this );

  _origin[0] += x;
  _origin[1] += y;
  _distance += z;

  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Modify the scale.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::scale ( const float &scale )
{
  SL_ASSERT ( this );
  _scale *= scale;
  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the scale.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::setScale ( const float &scale )
{
  SL_ASSERT ( this );
  _scale = scale;
  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  This does trackball transformations.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::_matrixCalc()
{
  SL_ASSERT ( this );

  // Put the camera ain the absolute global space with no scaling.
  _M.identity();

  // Note: when you reason this out, from here down you have to read 
  // bottom up. In other words, you first translate to the center of 
  // the world, then you rotate, ...

  // Back it up so we can see it.
  _M.translate ( SlVec3f ( 0.0f, 0.0f, -_distance ) );

  // Scale the world.
  _M.scale ( SlVec3f ( _scale, _scale, _scale ) );

  // Translation offset.
  _M.translate ( SlVec3f ( _origin[0], _origin[1], _origin[2] ) );

  // Rotate the world.
  _M.multRight ( _R );

  // Move to the center of rotaion.
  _M.translate ( SlVec3f ( -_center[0], -_center[1], -_center[2] ) );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the center of the camera.
//
/////////////////////////////////////////////////////////////////////////////

bool SgCamera::setCenter ( const SlVec3f &center, const bool &isGlobal )
{
  SL_ASSERT ( this );

  // If the given center is not in global space, it is assumed 
  // to be relative to this camera's local space.
  if ( !isGlobal )
  {
    // Get the inverse of our modelview matrix.
    SlMatrix4f IM = _M;
    if ( !IM.invert() ) 
      return false;

    // Set the center, and convert it to global space.
    _center = center;
    _center = IM * _center;
  }

  // Otherwise, it's already in global space.
  else _center = center;

  // Have to reset the translations.
  _origin.setValue ( 0.0f, 0.0f, 0.0f );

  // Recalculate the matrix.
  this->_matrixCalc();

  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
//  Look down the axis.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::lookDownX ( const float &sign )
{
  SL_ASSERT ( this );
  _R.identity();
  _R.rotate ( sign * SL_PI_OVER_2_F, SlVec3f ( 0.0f, 1.0f, 0.0f ) );
  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Look down the axis.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::lookDownY ( const float &sign )
{
  SL_ASSERT ( this );
  _R.identity();
  _R.rotate ( sign * SL_PI_OVER_2_F, SlVec3f ( 1.0f, 0.0f, 0.0f ) );
  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Look down the axis.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::lookDownZ ( const float &sign )
{
  SL_ASSERT ( this );
  _R.identity();

  // If the sign is positive then the call identity() is all we need.
  if ( sign < 0 )
    _R.rotate ( SL_PI_F, SlVec3f ( 0.0f, 1.0f, 0.0f ) );

  this->_matrixCalc();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Set the aspect ratio.
//
/////////////////////////////////////////////////////////////////////////////

void SgCamera::setAspectRatio ( const unsigned long &width, const unsigned long &height )
{
  SL_ASSERT ( this );
  SL_ASSERT ( width > 0 && height > 0 );
  this->setAspectRatio ( ( static_cast<float>(width) ) / ( static_cast<float>(height) ) );
}
