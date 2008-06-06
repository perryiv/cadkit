
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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

#include "Standard/SlVec3.h"
#include "Standard/SlMatrix44.h"


namespace CadKit
{
class SG_API SgCamera : public SgNode
{
public:

  enum MatrixMode
  {
    PROJECTION = 0x00000001, // Set the projection matrix when rendered.
    MODELVIEW  = 0x00000002  // Modify the modelview matrix when rendered (default).
  };

  const float &         getAspectRatio() const { return _aspect; }
  const SlVec3f &       getOrigin() const { return _origin; }
  const SlVec3f &       getCenter() const { return _center; }
  const float &         getDistance() const { return _distance; }
  const SlMatrix44f &    getModelviewMatrix() const { return _M; }
  SlMatrix44f &          getModelviewMatrix() { return _M; }
  virtual SlMatrix44f    getProjectionMatrix() const = 0;
  virtual void          getProjectionMatrix ( SlMatrix44f &P ) const = 0;
  const SlMatrix44f &    getRotation() const { return _R; }
  SlMatrix44f &          getRotation() { return _R; }
  const float &         getScale() const { return _scale; }
  const float &         getZNear() const { return _zNear; }
  const float &         getZFar() const { return _zFar; }
  
  // Move the camera so that the given scene is visible.
  virtual bool          viewAll ( SgNode &scene ) = 0;

  // Transform the camera.
  void                  rotate ( const SlMatrix44f &R );
  void                  translate ( const float &x, const float &y, const float &z );
  void                  scale ( const float &scale );

  // Set the aspect ratio.
  void                  setAspectRatio ( const unsigned long &width, const unsigned long &height );
  void                  setAspectRatio ( const float &aspect ) { _aspect = aspect; }

  // Set the center of rotation. Pass false for "isGlobal" if the center 
  // is with respect to the camera's local space.
  bool                  setCenter ( const SlVec3f &center, const bool &isGlobal );

  // Set the camera's transformation.
  bool                  setRotation ( const SlMatrix44f &R );
  void                  setScale ( const float &scale );
  void                  setMatrix ( const SlMatrix44f &M ) { _M.setValue ( M ); }

  // Set the matrix mode.
  void                  setMatrixMode ( const unsigned long &mode ) { _mode = mode; }

  // Set the data members from the given camera.
  void                  setValue ( const SgCamera &camera );

  // Set/get the scale factor for the viewAll() function.
  void                  setViewAllFactor ( const float &factor ) { _viewAllFactor = factor; }
  const float &         getViewAllFactor() const { return _viewAllFactor; }

  // Set the z-near and z-far distance.
  void                  setZNear ( const float &zNear ) { _zNear = zNear; }
  void                  setZFar  ( const float &zFar )  { _zFar  = zFar; }

  // Look down the axis.
  void                  lookDownX ( const float &sign );
  void                  lookDownY ( const float &sign );
  void                  lookDownZ ( const float &sign );

protected:

  SlUint32 _mode;
  float _aspect;
  float _zNear;
  float _zFar;
  float _scale;
  SlVec3f _origin;
  SlVec3f _center;
  float _distance;
  SlMatrix44f _R;
  SlMatrix44f _M;
  float _viewAllFactor; // Scale factor for the viewAll() function to use.

  SgCamera();
  SgCamera ( const SgCamera &camera );
  virtual ~SgCamera();

  void          _matrixCalc();

  SG_DECLARE_NODE ( SgCamera, 1033952050 );
};
};

#endif
