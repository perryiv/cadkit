
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Preference file parser.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_PREFS_READER_CLASS_H_
#define _VRV_PREFS_READER_CLASS_H_

#include "VRV/Prefs/Settings.h"

#include "Usul/Errors/ThrowingPolicy.h"

#include <string>


namespace VRV {
namespace Prefs {

class Settings;

class Parser
{
public:

  Parser();
  ~Parser();

  // Read the user-preferences file.
  void                  read ( const std::string &filename, Settings *settings );

protected:

  typedef Usul::Errors::ThrowingPolicy<std::runtime_error> ErrorChecker;

  // Read the data from file.
  void                  _read ( const std::string &filename );

  // Callbacks for the xml-reader.
  void                  _addGrid           ( const std::string & );
  void                  _setNumGridBlocks  ( const std::string & );
  void                  _setGridScale      ( const std::string & );
  void                  _setGridColor      ( const std::string & );
  void                  _setGridFillColor      ( const std::string & );
  void                  _setGridRotationAngleRad ( const std::string & );
  void                  _setGridRotationVector   ( const std::string & );
  void                  _offsetGrid        ( const std::string & );
  void                  _setNearClip       ( const std::string & );
  void                  _setViewAllScale   ( const std::string & );
  void                  _setLightAmbient   ( const std::string & );
  void                  _setLightDiffuse   ( const std::string & );
  void                  _setLightSpecular  ( const std::string & );
  void                  _setLightDirection ( const std::string & );
  void                  _setLightPosition  ( const std::string & );
  void                  _setBackground     ( const std::string & );
  void                  _setWriter         ( const std::string & );
  void                  _setHeadNode       ( const std::string & );
  void                  _setNormGlobal     ( const std::string & );
  void                  _setNormModels     ( const std::string & );
  void                  _translateMenu     ( const std::string & );
  void                  _scaleMenu         ( const std::string & );
  void                  _setMenuBgColorNorm   ( const std::string & );
  void                  _setMenuBgColorHLght  ( const std::string & );
  void                  _setMenuBgColorDsabl  ( const std::string & );
  void                  _setMenuTxtColorNorm  ( const std::string & );
  void                  _setMenuTxtColorHLght ( const std::string & );
  void                  _setMenuTxtColorDsabl ( const std::string & );
  void                  _translateStatus   ( const std::string & );
  void                  _scaleStatus       ( const std::string & );
  void                  _setStatusVisible  ( const std::string & );
  void                  _setMenuHidesScene ( const std::string & );
  void                  _setTransSpeed     ( const std::string & );
  void                  _setRotSpeed       ( const std::string & );
  void                  _setScaleSpeed     ( const std::string & );
  void                  _setSinterServer   ( const std::string & );
  void                  _setSinterWriter   ( const std::string & );
  void                  _setSinterTmpFile  ( const std::string & );
  void                  _setScribeColor    ( const std::string & );
  void                  _setScribeWidth    ( const std::string & );
  void                  _setAutoPlacement  ( const std::string & );
  void                  _setAutoPlaceCenter ( const std::string & );
  void                  _setAutoPlaceRadius ( const std::string & );
  void                  _setNavInvertRotation ( const std::string & );
  void                  _setNavNewRotation    ( const std::string & );
  void                  _setNavAcceleration   ( const std::string & );
  void                  _setNavIconColor      ( const std::string & );
  void                  _setNavIconSize       ( const std::string & );
  void                  _setAutoRotationAngle ( const std::string & );
  void                  _setAutoRotationVector( const std::string & );
  void                  _setUserName          ( const std::string & );
  void                  _setAvatarWaitFrames  ( const std::string & );

private:

  // Do not copy.
  Parser ( const Parser & );
  Parser &operator = ( const Parser & );

  Settings::ValidAccessRefPtr _settings;
};


}; // namespace Prefs
}; // namespace VRV


#endif // _VRV_PREFS_READER_CLASS_H_
