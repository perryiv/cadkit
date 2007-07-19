
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  User settings.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_SETTINGS_CLASS_H_
#define _VRV_SETTINGS_CLASS_H_

#include "VRV/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Absolute.h"

#include <string>
#include <vector>

namespace VRV {
namespace Prefs {

class Grids
{
  public:
  Usul::Math::Vec2ui _numGridBlocks;
  Usul::Math::Vec2f _gridScale;
  Usul::Math::Vec4f _gridColor;
  Usul::Math::Vec4f _gridFillColor;
  float _gridRotationAngleRad;
  Usul::Math::Vec3f _gridRotationVector;
  bool _offsetGrid;
};


class VRV_EXPORT Settings : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Math::Vec4f Color;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4f Vec4f;
  typedef Usul::Math::Matrix44f Matrix;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Settings );

  // Construct/destruct.
  Settings();

  // Read the user-settings file.
  void                  read ( const std::string &filename );

  // Set/get the grid properties.
  int                   numGrids() { return _grids.size(); }
  void                  addGrid() { _grids.push_back(Grids()); }
  const Color &         gridColor ( int i ) const { return _grids[i]._gridColor; }
  void                  gridColor (const Color &c ) { _grids[_grids.size()-1]._gridColor = c; }
  const Color &         gridFillColor ( int i ) const { return _grids[i]._gridFillColor; }
  void                  gridFillColor (const Color &c ) { _grids[_grids.size()-1]._gridFillColor = c; }
  const Vec2f &         gridScale ( int i ) const { return _grids[i]._gridScale; }
  void                  gridScale ( const Vec2f & s ) { _grids[_grids.size()-1]._gridScale = s; }
  const Vec2ui &        numGridBlocks ( int i ) const { return _grids[i]._numGridBlocks; }
  void                  numGridBlocks ( const Vec2ui &n ) { _grids[_grids.size()-1]._numGridBlocks = n; }
  float                 gridRotationAngleRad ( int i ) { return _grids[i]._gridRotationAngleRad; }
  void                  gridRotationAngleRad ( float a ) { _grids[_grids.size()-1]._gridRotationAngleRad = a; }
  const Vec3f &         gridRotationVector ( int i ) { return _grids[i]._gridRotationVector; }
  void                  gridRotationVector ( Vec3f v ) { _grids[_grids.size()-1]._gridRotationVector = v; }
  bool                  offsetGrid( int i ) { return _grids[i]._offsetGrid; }
  void                  offsetGrid( bool b ) { _grids[_grids.size()-1]._offsetGrid = b; }
  
  // Clipping plane distances.
  float                 nearClippingDistance() const { return _zNear; }
  void                  nearClippingDistance ( float zNear ) { _zNear = zNear; }

  /// Far plane multiplier.
  float                 farClippingPlaneMultiplier () const { return _zFarPlaneMultiplier; }
  void                  farClippingPlaneMultiplier ( float value ) { _zFarPlaneMultiplier = value; }

  // The scale factor that is multiplied by the bounding-sphere radius. 
  // The resulting number is the distance in the z-direction the node 
  // is moved in order to make it visible.
  float                 viewAllScaleZ() const { return _zScale; }
  void                  viewAllScaleZ ( float zScale ) { _zScale = zScale; }

  // Get/set the lighting properties.
  const Vec4f &         lightPosition() const         { return _lightPos; }
  void                  lightPosition(const Vec4f& p) { _lightPos = p; }
  const Color &         ambientLightColor() const { return _ambientLight; }
  void                  ambientLightColor ( const Vec4f &c ) { _ambientLight = c; }
  const Color &         diffuseLightColor() const { return _diffuseLight; }
  void                  diffuseLightColor ( const Vec4f &c ) { _diffuseLight = c; }
  const Color &         specularLightColor() const { return _specularLight; }
  void                  specularLightColor ( const Vec4f &c ) { _specularLight = c; }
  const Vec3f &         lightDirection() const { return _lightDir; }
  void                  lightDirection ( const Vec3f &c ) { _lightDir = c; }

  // Set/get the background color.
  const Color &         backgroundColor() const { return _background; }
  void                  backgroundColor ( const Color &c ) { _background = c; }
  void                  backgroundColor ( float r, float g, float b, float a ) { _background.set ( r, g, b, a ); }

  // Set/get the machine that will write files.
  const std::string &   fileWriterMachineName() const { return _writer; }
  void                  fileWriterMachineName ( const std::string &writer ) { _writer = writer; }

  // Set/get the machine that is the head node.
  const std::string &   headNodeMachineName() const { return _headNode; }
  void                  headNodeMachineName ( const std::string &head ) { _headNode = head; }
  

  // Set/get the global-normalization flag.
  bool                  normalizeVertexNormalsGlobal() const { return _normGlobal; }
  void                  normalizeVertexNormalsGlobal ( bool state ) { _normGlobal = state; }

  // Set/get the model-normalization flag.
  bool                  normalizeVertexNormalsModels() const { return _normModels; }
  void                  normalizeVertexNormalsModels ( bool state ) { _normModels = state; }

  // Set/get the menu matrix.
  const Matrix &        menuMatrix() const { return _menuMatrix; }
  Matrix &              menuMatrix()       { return _menuMatrix; }
  void                  menuMatrix ( const Matrix &m ) { _menuMatrix = m; }

  // Set/get the menu colors
  const Color&          menuBgColorNorm  () const { return _menuBgColorNorm; }               // background normal
  void                  menuBgColorNorm  ( const Color &c ) { _menuBgColorNorm = c; }
  void                  menuBgColorNorm  ( float r, float g, float b, float a ) { _menuBgColorNorm.set ( r, g, b, a ); }
  const Color&          menuBgColorHLght () const { return _menuBgColorHLght; }              // background highlighted
  void                  menuBgColorHLght ( const Color &c ) { _menuBgColorHLght = c; }
  void                  menuBgColorHLght ( float r, float g, float b, float a ) { _menuBgColorHLght.set ( r, g, b, a ); }
  const Color&          menuBgColorDsabl () const { return _menuBgColorDsabl; }              // background disabled
  void                  menuBgColorDsabl ( const Color &c ) { _menuBgColorDsabl = c; }
  void                  menuBgColorDsabl ( float r, float g, float b, float a ) { _menuBgColorDsabl.set ( r, g, b, a ); }
  const Color&          menuTxtColorNorm  () const { return _menuTxtColorNorm; }             // background normal
  void                  menuTxtColorNorm  ( const Color &c ) { _menuTxtColorNorm = c; }
  void                  menuTxtColorNorm  ( float r, float g, float b, float a ) { _menuTxtColorNorm.set ( r, g, b, a ); }
  const Color&          menuTxtColorHLght () const { return _menuTxtColorHLght; }            // background highlighted
  void                  menuTxtColorHLght ( const Color &c ) { _menuTxtColorHLght = c; }
  void                  menuTxtColorHLght ( float r, float g, float b, float a ) { _menuTxtColorHLght.set ( r, g, b, a ); }
  const Color&          menuTxtColorDsabl () const { return _menuTxtColorDsabl; }            // background disabled
  void                  menuTxtColorDsabl ( const Color &c ) { _menuTxtColorDsabl = c; }
  void                  menuTxtColorDsabl ( float r, float g, float b, float a ) { _menuTxtColorDsabl.set ( r, g, b, a ); }

  // Set/get the status-bar matrix.
  const Matrix &        statusBarMatrix() const { return _statusMatrix; }
  Matrix &              statusBarMatrix()       { return _statusMatrix; }
  void                  statusBarMatrix ( const Matrix &m ) { _statusMatrix = m; }

  // Set/get the flag that says whether or not the menu hides the scene.
  bool                  menuHidesScene() const { return _menuHidesScene; }
  void                  menuHidesScene ( bool state ) { _menuHidesScene = state; }

  // Set/get the flag that says whether or not the status-bar is visible at startup.
  bool                  statusBarVisibleAtStartup() const { return _statusVisible; }
  void                  statusBarVisibleAtStartup ( bool state ) { _statusVisible = state; }

  // Set/get the status bar colors
  const Color&          statusBgColor () const { return _statusBgColor; }
  void                  statusBgColor ( const Color &c ) { _statusBgColor = c; }
  void                  statusBgColor ( float r, float g, float b, float a ) { _statusBgColor.set ( r, g, b, a ); }
  const Color&          statusTxtColor () const {return _statusTxtColor; }
  void                  statusTxtColor ( const Color &c ) { _statusTxtColor = c; }
  void                  statusTxtColor ( float r, float g, float b, float a ) { _statusTxtColor.set ( r, g, b, a ); }

  // Set/get the relative translation speed.
  float                 translationSpeed() const { return _transSpeed; }
  void                  translationSpeed ( float s ) { _transSpeed = s; }

  // Set/get the rotation speed.
  float                 rotationSpeed() const { return _rotSpeed; }
  void                  rotationSpeed ( float s ) { _rotSpeed = s; }

  // Set/get the scale-speed.
  float                 scaleSpeed() const { return _scaleSpeed; }
  void                  scaleSpeed ( float s ) { _scaleSpeed = s; }

  // Set/get the selection color.
  const Color &         selectionColor() const { return _selectColor; }
  void                  selectionColor ( const Color &c ) { _selectColor = c; }
  void                  selectionColor ( float r, float g, float b, float a ) { _selectColor.set ( r, g, b, a ); }  
  
  /// Get/Set the frame scale multiplier.
  void                  frameScale ( float f ) { _frameScale = f; }
  float                 frameScale () const { return _frameScale; }

  /// Get/Set the image export extension.
  void                  imageExportExtension ( const std::string& ext ) { _imageExportExtension = ext; }
  const std::string&    imageExportExtension ( ) const { return _imageExportExtension; }

protected:

  // Use reference counting.
  virtual ~Settings();

private:

  // No copying or assigning.
  Settings ( const Settings & );
  Settings &operator = ( const Settings & );

  std::vector<Grids> _grids;
  float _zNear;
  float _zFarPlaneMultiplier;
  float _zScale;
  Color _ambientLight;
  Color _diffuseLight;
  Color _specularLight;
  Vec3f _lightDir;
  Vec4f _lightPos;
  Color _background;
  std::string _writer;
  std::string _headNode;
  bool _normGlobal;
  bool _normModels;
  Matrix _menuMatrix;
  Color _menuBgColorNorm;
  Color _menuBgColorHLght;
  Color _menuBgColorDsabl;
  Color _menuTxtColorNorm;
  Color _menuTxtColorHLght;
  Color _menuTxtColorDsabl;
  Matrix _statusMatrix;
  bool _menuHidesScene;
  bool _statusVisible;
  Color _statusBgColor;
  Color _statusTxtColor;
  float _transSpeed;
  float _rotSpeed;
  float _scaleSpeed;
  Color _selectColor;
  float _frameScale;
  std::string _imageExportExtension;
};


}; // namespace Prefs
}; // namespace VRV


#endif // _VRV_SETTINGS_CLASS_H_
