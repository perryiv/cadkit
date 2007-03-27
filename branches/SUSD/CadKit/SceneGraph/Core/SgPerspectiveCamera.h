
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
  float                   getFieldOfView() const { return this->getFieldOfView ( fieldOfViewOrientation ); }
  float                   getFieldOfView ( const FieldOfViewOrientation &flag ) const;

  // Get the projection matrix.
  virtual SlMatrix44f      getProjectionMatrix() const;
  virtual void            getProjectionMatrix ( SlMatrix44f &P ) const;

  // Set the field of view.
  void                    setFieldOfView ( const float &fov ) { _fieldOfView = fov; }

  // Move the camera so that the given scene is visible.
  virtual bool            viewAll ( SgNode &scene );

protected:

  float _fieldOfView;

  virtual ~SgPerspectiveCamera();

  SG_DECLARE_DYNAMIC_NODE ( SgPerspectiveCamera, 1033952440 );
};
};

#endif
