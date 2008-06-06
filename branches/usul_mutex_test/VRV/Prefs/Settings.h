
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
#include "Usul/Registry/Database.h"
#include "Usul/Strings/Split.h"

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
  typedef Usul::Math::Vec3d Vec3d;
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
  float                 nearClippingDistance() const;
  void                  nearClippingDistance ( float zNear );

  /// Far plane multiplier.
  float                 farClippingPlaneMultiplier () const;
  void                  farClippingPlaneMultiplier ( float value );

  // The scale factor that is multiplied by the bounding-sphere radius. 
  // The resulting number is the distance in the z-direction the node 
  // is moved in order to make it visible.
  float                 viewAllScaleZ() const;
  void                  viewAllScaleZ ( float zScale );

  // Get/set the lighting properties.
  Vec4f                 lightPosition() const;
  void                  lightPosition(const Vec4f& p);
  Color                 ambientLightColor() const;
  void                  ambientLightColor ( const Vec4f &c );
  Color                 diffuseLightColor() const;
  void                  diffuseLightColor ( const Vec4f &c );
  Color                 specularLightColor() const;
  void                  specularLightColor ( const Vec4f &c );
  Vec3f                 lightDirection() const;
  void                  lightDirection ( const Vec3f &c );

  // Set/get the background color.
  Color                 backgroundColor() const;
  void                  backgroundColor ( const Color &c );

  // Set/get the machine that will write files.
  std::string           fileWriterMachineName() const;
  void                  fileWriterMachineName ( const std::string &writer );

  // Set/get the machine that is the head node.
  std::string           headNodeMachineName() const;
  void                  headNodeMachineName ( const std::string &head );
  

  // Set/get the global-normalization flag.
  bool                  normalizeVertexNormalsGlobal() const;
  void                  normalizeVertexNormalsGlobal ( bool state );

  // Set/get the model-normalization flag.
  bool                  normalizeVertexNormalsModels() const;
  void                  normalizeVertexNormalsModels ( bool state );

  // Set/get the menu matrix.
  const Matrix          menuMatrix() const;
  Matrix                menuMatrix();
  void                  menuMatrix ( const Matrix &m );

  // Set/get the menu colors
  Color                 menuBgColorNorm  () const;
  void                  menuBgColorNorm  ( const Color &c );

  Color                 menuBgColorHLght () const;
  void                  menuBgColorHLght ( const Color &c );

  Color                 menuBgColorDsabl () const;
  void                  menuBgColorDsabl ( const Color &c );

  Color                 menuTxtColorNorm  () const;
  void                  menuTxtColorNorm  ( const Color &c );

  Color                 menuTxtColorHLght () const;
  void                  menuTxtColorHLght ( const Color &c );

  Color                 menuTxtColorDsabl () const;
  void                  menuTxtColorDsabl ( const Color &c );

  // Set/get the status-bar matrix.
  const Matrix          statusBarMatrix() const;
  Matrix                statusBarMatrix();
  void                  statusBarMatrix ( const Matrix &m );

  // Set/get the flag that says whether or not the status-bar is visible at startup.
  bool                  statusBarVisibleAtStartup() const;
  void                  statusBarVisibleAtStartup ( bool state );

  // Set/get the status bar colors
  Color                 statusBgColor () const;
  void                  statusBgColor ( const Color &c );

  Color                 statusTxtColor () const;
  void                  statusTxtColor ( const Color &c );

  // Set/get the relative translation speed.
  float                 translationSpeed() const;
  void                  translationSpeed ( float s );

  // Set/get the rotation speed.
  float                 rotationSpeed() const;
  void                  rotationSpeed ( float s );

  // Set/get the scale-speed.
  float                 scaleSpeed() const;
  void                  scaleSpeed ( float s );

  // Set/get the selection color.
  Color                 selectionColor() const;
  void                  selectionColor ( const Color &c );
  
  /// Get/Set the frame scale multiplier.
  void                  frameScale ( float f );
  float                 frameScale () const;

  /// Get/Set the image export extension.
  void                  imageExportExtension ( const std::string& ext );
  std::string           imageExportExtension ( ) const;

  /// Get/set the image directory.
  void                  imageDirectory ( const std::string& s );
  std::string           imageDirectory() const;

  /// Get/set the wand offset.
  void                  wandOffset ( const Vec3d& );
  Vec3d                 wandOffset() const;

  template<class T>
  T                     value ( const std::string& path, const T& defaultValue ) const;

  template<class T>
  void                  value ( const std::string& path, const T& value );

protected:

  // Use reference counting.
  virtual ~Settings();

private:

  void                  _read ( const std::string& filename );

  // No copying or assigning.
  Settings ( const Settings & );
  Settings &operator = ( const Settings & );

  Usul::Registry::Database _database;
  std::vector<Grids> _grids;
  Color _selectColor;
};

template<class T>
T Settings::value ( const std::string& path, const T& defaultValue ) const
{
  typedef Usul::Registry::Node Node;
  typedef Node::Path Path;

  Path p;
  Usul::Strings::split ( path, "/", false, p );

  const Node* child ( _database.root() );

  for ( Path::const_iterator iter = p.begin(); iter != p.end(); ++iter )
  {
    if ( 0x0 != child )
      child = child->find( *iter );
  }

  return ( 0x0 != child ? child->get<T> ( defaultValue ) : defaultValue );
}

template<class T>
void Settings::value ( const std::string& path, const T& value )
{
  typedef Usul::Registry::Node Node;
  typedef Node::Path Path;

  Path p;
  Usul::Strings::split ( path, "/", false, p );
  
  _database[p] = value;
}

} // namespace Prefs
} // namespace VRV


#endif // _VRV_SETTINGS_CLASS_H_
