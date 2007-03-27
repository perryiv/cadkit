
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//  SgCamera.h: Orthographic camera class.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_SCENEGRAPH_CORE_LIBRARY_ORTHOGRAPHIC_CAMERA_H_
#define _CADKIT_SCENEGRAPH_CORE_LIBRARY_ORTHOGRAPHIC_CAMERA_H_

#include "SgCamera.h"


namespace CadKit
{
class SG_API SgOrthographicCamera : public SgCamera
{
public:

  SgOrthographicCamera();
  SgOrthographicCamera ( const SgOrthographicCamera &camera );

  // Get the height.
  const float &           getHeight() const { return _height; }

  // Get the projection matrix.
  virtual SlMatrix44f      getProjectionMatrix() const;
  virtual void            getProjectionMatrix ( SlMatrix44f &P ) const;

  // Move the camera so that the given scene is visible.
  virtual bool            viewAll ( SgNode &scene );


protected:

  float _height;

  virtual ~SgOrthographicCamera();

  SG_DECLARE_DYNAMIC_NODE ( SgOrthographicCamera, 1033952427 );
};
};

#endif
