
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

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_CAMERA_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_CAMERA_H_

#include "SgNode.h"
#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlVec3.h"
# include "Standard/SlMatrix4.h"
#endif

namespace CadKit
{
class SG_API SgCamera : public SgNode
{
public:

  enum MatrixMode
  {
    PROJECTION = (1L<<0), // Set the projection matrix when rendered.
    MODELVIEW =  (1L<<1)  // Modify the modelview matrix when rendered (default).
  };

  const float &         getAspectRatio() const { return _aspect; }
  const SlVec3f &       getOrigin() const { return _origin; }
  const SlVec3f &       getCenter() const { return _center; }
  const SlFloat32 &     getDistance() const { return _distance; }
  const SlMatrix4f &    getModelviewMatrix() const { return _M; }
  SlMatrix4f &          getModelviewMatrix() { return _M; }
  virtual SlMatrix4f    getProjectionMatrix() const = 0;
  virtual void          getProjectionMatrix ( SlMatrix4f &P ) const = 0;
  const SlMatrix4f &    getRotation() const { return _R; }
  SlMatrix4f &          getRotation() { return _R; }
  const float &         getScale() const { return _scale; }
  const float &         getZNear() const { return _zNear; }
  const float &         getZFar() const { return _zFar; }
  
  // Move the camera so that the given scene is visible.
  virtual bool          viewAll ( SgNode &scene ) = 0;

  // Transform the camera.
  void                  rotate ( const SlMatrix4f &R );
  void                  translate ( const float &x, const float &y, const float &z );
  void                  scale ( const float &scale );

  // Set the aspect ratio.
  void                  setAspectRatio ( const unsigned long &width, const unsigned long &height );
  void                  setAspectRatio ( const float &aspect ) { _aspect = aspect; }

  // Set the center of rotation. Pass false for "isGlobal" if the center 
  // is with respect to the camera's local space.
  bool                  setCenter ( const SlVec3f &center, const bool &isGlobal );

  // Set the camera's transformation.
  bool                  setRotation ( const SlMatrix4f &R );
  void                  setScale ( const float &scale );
  void                  setMatrix ( const SlMatrix4f &M ) { _M.setValue ( M ); }

  // Set the matrix mode.
  void                  setMatrixMode ( const unsigned long &mode ) { _mode = mode; }

  // Set the data members from the given camera.
  void                  setValue ( const SgCamera &camera );

  // Set/get the scale factor for the viewAll() function.
  void                  setViewAllFactor ( const float &factor ) { _viewAllFactor = factor; }
  const SlFloat32 &     getViewAllFactor() const { return _viewAllFactor; }

  // Set the z-near and z-far distance.
  void                  setZNear ( const float &zNear ) { _zNear = zNear; }
  void                  setZFar  ( const float &zFar )  { _zFar  = zFar; }

  // Look down the axis.
  void                  lookDownX ( const float &sign );
  void                  lookDownY ( const float &sign );
  void                  lookDownZ ( const float &sign );

protected:

  SlUint32 _mode;
  SlFloat32 _aspect;
  SlFloat32 _zNear;
  SlFloat32 _zFar;
  SlFloat32 _scale;
  SlVec3f _origin;
  SlVec3f _center;
  SlFloat32 _distance;
  SlMatrix4f _R;
  SlMatrix4f _M;
  SlFloat32 _viewAllFactor; // Scale factor for the viewAll() function to use.

  SgCamera();
  SgCamera ( const SgCamera &camera );
  virtual ~SgCamera();

  void          _matrixCalc();

  SG_DECLARE_NODE(SgCamera,0x00001005);
};
};

#endif
