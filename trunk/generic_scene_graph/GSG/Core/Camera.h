
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

#ifndef _GENERIC_SCENE_GRAPH_CORE_CAMERA_H_
#define _GENERIC_SCENE_GRAPH_CORE_CAMERA_H_

#include "GSG/Core/Group.h"


namespace GSG {

class Viewer;


class GSG_CORE_EXPORT Camera : public Group
{
public:

  GSG_DECLARE_REFERENCED ( Camera );
  GSG_DECLARE_LOCAL_TYPEDEFS ( Camera, Group );

  explicit Camera();
  Camera ( const Camera &c );

  // Get the matrices.
  const Matrix &      modelview()  const;
  const Matrix &      projection() const;

  // Transform the camera.
  void                rotate ( const Matrix &R );
  void                translate ( const Vec3 &t );
  void                scale ( Real scale );

  // Get these properties.
  Real                fieldOfView() const { return _fov; }
  Real                aspectRatio() const { return _aspect; }
  Real                nearZ()       const { return _nearZ; }
  Real                farZ()        const { return _farZ; }

  // Set these properties.
  void                fieldOfView ( Real aspect );
  void                aspectRatio ( Real aspect );
  void                aspectRatio ( UnsignedInteger width, UnsignedInteger height );
  void                nearZ       ( Real aspect );
  void                farZ        ( Real aspect );

  // Move the camera such that all of the visible children fall within 
  // the viewing frustum.
  void                viewAll();

protected:

  virtual ~Camera();

  void                _updateMatrices();

  GSG_DECLARE_ACCEPT ( Camera );

private:

  bool _valid;
  Matrix _projection;
  Matrix _modelview;
  Real _fov;
  Real _aspect;
  Real _nearZ;
  Real _farZ;
  Real _scale;
  Vec3 _origin;
  Vec3 _center;
  Real _distance;
  Matrix _rotation;
  Real _vaFactor;
};


}; // namespace GSG


#endif // _GENERIC_SCENE_GRAPH_CORE_CAMERA_H_
