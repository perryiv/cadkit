
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
//  SgCamera.h: Perspective camera class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_PERSPECTIVE_CAMERA_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_PERSPECTIVE_CAMERA_H_

#include "SgCamera.h"

namespace CadKit
{
class SG_API SgPerspectiveCamera : public SgCamera
{
public:

  enum FieldOfViewOrientation
  {
    FOV_IN_Y,   // The field of view is in the y-direction.
    FOV_IN_X,   // The field of view is in the x-direction.
    FOV_IN_MIN, // The field of view is in either the x- or y-direction, whichever is smaller.
    FOV_IN_MAX  // The field of view is in either the x- or y-direction, whichever is bigger.
  } fieldOfViewOrientation;

  SgPerspectiveCamera();
  SgPerspectiveCamera ( const SgPerspectiveCamera &camera );

  // Get the proper field of view for the projection, 
  // based on fieldOfViewOrientation and fieldOfView.
  SlFloat32               getFieldOfView() const { return this->getFieldOfView ( fieldOfViewOrientation ); }
  SlFloat32               getFieldOfView ( const FieldOfViewOrientation &flag ) const;

  // Get the projection matrix.
  virtual SlMatrix4f      getProjectionMatrix() const;
  virtual void            getProjectionMatrix ( SlMatrix4f &P ) const;

  // Set the field of view.
  void                    setFieldOfView ( const SlFloat32 &fov ) { _fieldOfView = fov; }

  // Move the camera so that the given scene is visible.
  virtual bool            viewAll ( SgNode &scene );

protected:

  SlFloat32 _fieldOfView;

  virtual ~SgPerspectiveCamera();

  SG_DECLARE_DYNAMIC_NODE(SgPerspectiveCamera,0x00001020);
};
};

#endif
