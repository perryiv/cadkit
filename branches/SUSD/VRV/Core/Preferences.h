
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User preferences.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VIRTUAL_REALITY_VIEWER_PREFERENCES_CLASS_H_
#define _VIRTUAL_REALITY_VIEWER_PREFERENCES_CLASS_H_

#include "VRV/Core/Export.h"

#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

#include <string>


namespace VRV {


class Preferences
{
public:

  // Typedefs.
  typedef Usul::Math::Vec4f Color;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4f Vec4f;

  // Construct/destruct.
  Preferences();
  virtual ~Preferences();

  // Read the user-preferences file.
  void                  read ( const std::string &filename );

  // Set/get the properties.
  const Color &         gridColor() const { return _gridColor; }
  void                  gridColor ( const Color &c ) { _gridColor = c; }
  const Vec2f &         gridScale() const { return _gridScale; }
  void                  gridScale ( const Vec2f & s ) { _gridScale = s; }
  const Vec2ui &        numGridBlocks() const { return _numGridBlocks; }
  void                  numGridBlocks ( const Vec2ui &n ) { _numGridBlocks = n; }

  // Clipping plane distances.
  float                 nearClippingDistance() const { return _zNear; }
  void                  nearClippingDistance ( float zNear ) { _zNear = zNear; }

  // The scale factor that is multiplied by the bounding-sphere radius. 
  // The resulting number is the distance in the z-direction the node 
  // is moved in order to make it visible.
  float                 viewAllScaleZ() const { return _zScale; }
  void                  viewAllScaleZ ( float zScale ) { _zScale = zScale; }

  // Get/set the lighting properties.
  const Vec4f &         ambientLightColor() const { return _ambientLight; }
  void                  ambientLightColor ( const Vec4f &c ) { _ambientLight = c; }
  const Vec4f &         diffuseLightColor() const { return _diffuseLight; }
  void                  diffuseLightColor ( const Vec4f &c ) { _diffuseLight = c; }
  const Vec4f &         specularLightColor() const { return _specularLight; }
  void                  specularLightColor ( const Vec4f &c ) { _specularLight = c; }
  const Vec3f &         lightDirection() const { return _lightDir; }
  void                  lightDirection ( const Vec3f &c ) { _lightDir = c; }

private:

  // No copying or assigning.
  Preferences ( const Preferences & );
  Preferences &operator = ( const Preferences & );

  Vec2ui _numGridBlocks;
  Vec2f _gridScale;
  Color _gridColor;
  float _zNear;
  float _zScale;
  Vec4f _ambientLight;
  Vec4f _diffuseLight;
  Vec4f _specularLight;
  Vec3f _lightDir;
};


}; // namespace VRV


#endif // _VIRTUAL_REALITY_VIEWER_PREFERENCES_CLASS_H_
