
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

#include "VRV/Prefs/Settings.h"
#include "VRV/Common/Constants.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Matrix44.h"
#include "Usul/Convert/Vector4.h"
#include "Usul/Convert/Vector3.h"
#include "Usul/File/Temp.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/Host.h"

#include "XmlTree/RegistryIO.h"

using namespace VRV;
using namespace VRV::Prefs;

namespace Sections = VRV::Constants::Sections;
namespace Keys = VRV::Constants::Keys;
namespace Defaults = VRV::Constants::Defaults;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Settings()  : 
  BaseClass(),
  _database ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Settings::~Settings()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the user-settings file.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::read ( const std::string &filename )
{
  // Read the database...
  Usul::Functions::safeCallR1 ( Usul::Adaptors::memberFunction<void> ( this, &Settings::_read ), filename, "3479583449" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the user-settings file.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::_read ( const std::string& filename )
{
  // Clear what we have.
  _database.clear();

  // Read the database.
  XmlTree::RegistryIO::read ( filename, _database );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the near clippling place distance.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::nearClippingDistance() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::CLIPPING_PLANE, "/", Keys::NEAR_PLANE ) );

  return this->value<float> ( path, Defaults::NEAR_CLIPPING_PLANE );

  // This won't compile because the [] operator isn't const.
  //return _database[Sections::PREFERENCES][Keys::CLIPPING_PLANE][Keys::NEAR].get<float> ( Defaults::NEAR_CLIPPING_PLANE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the near clippling place distance.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::nearClippingDistance ( float zNear )
{
  _database[Sections::PREFERENCES][Keys::CLIPPING_PLANE][Keys::NEAR_PLANE] = zNear;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the far clippling place multiplier.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::farClippingPlaneMultiplier () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::CLIPPING_PLANE, "/", Keys::FAR_MULTIPLIER ) );

  return this->value<float> ( path, Defaults::FAR_MULTIPLIER );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the far clippling place multiplier.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::farClippingPlaneMultiplier ( float value )
{
  _database[Sections::PREFERENCES][Keys::CLIPPING_PLANE][Keys::FAR_MULTIPLIER] = value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get view all scale.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::viewAllScaleZ() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::VIEW_ALL, "/", Keys::SCALE ) );

  return this->value<float> ( path, Defaults::VIEW_ALL_SCALE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set view all scale.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::viewAllScaleZ ( float zScale )
{
  _database[Sections::PREFERENCES][Keys::VIEW_ALL][Keys::SCALE] = zScale;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light position.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Vec4f  Settings::lightPosition() const 
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::LIGHT, "/", Keys::POSITION ) );
  return this->value<Vec4f> ( path, Defaults::LIGHT_POSITION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light position.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::lightPosition( const Vec4f& p )
{
  _database[Sections::PREFERENCES][Keys::LIGHT][Keys::POSITION] = p;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the ambient light color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::ambientLightColor() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::LIGHT, "/", Keys::AMBIENT ) );
  return this->value<Color> ( path, Defaults::LIGHT_AMBIENT );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the ambient light color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::ambientLightColor ( const Vec4f &c )
{
  _database[Sections::PREFERENCES][Keys::LIGHT][Keys::AMBIENT] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the diffuse light color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::diffuseLightColor() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::LIGHT, "/", Keys::DIFFUSE ) );
  return this->value<Color> ( path, Defaults::LIGHT_DIFFUSE );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the diffuse light color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::diffuseLightColor ( const Vec4f &c )
{
  _database[Sections::PREFERENCES][Keys::LIGHT][Keys::DIFFUSE] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the specular light color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::specularLightColor() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::LIGHT, "/", Keys::SPECULAR ) );
  return this->value<Color> ( path, Defaults::LIGHT_SPECULAR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the specular light color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::specularLightColor ( const Vec4f &c )
{
  _database[Sections::PREFERENCES][Keys::LIGHT][Keys::SPECULAR] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the light direction.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Vec3f Settings::lightDirection() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::LIGHT, "/", Keys::DIRECTION ) );
  return this->value<Vec3f> ( path, Defaults::LIGHT_DIRECTION );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light direction.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::lightDirection ( const Vec3f &c )
{
  _database[Sections::PREFERENCES][Keys::LIGHT][Keys::DIRECTION] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the background color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::backgroundColor() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::BACKGROUND, "/", Keys::COLOR ) );
  return this->value<Color> ( path, Defaults::BACKGROUND_COLOR );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::backgroundColor ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::BACKGROUND][Keys::COLOR] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the machine that will write files.
//
///////////////////////////////////////////////////////////////////////////////

std::string Settings::fileWriterMachineName() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::MACHINE, "/", Keys::WRITER ) );
  return this->value<std::string> ( path, Usul::System::Host::name() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the machine that will write files.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::fileWriterMachineName ( const std::string &writer )
{
  _database[Sections::PREFERENCES][Keys::MACHINE][Keys::WRITER] = writer;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the machine that is the head node.
//
///////////////////////////////////////////////////////////////////////////////

std::string Settings::headNodeMachineName() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::MACHINE, "/", Keys::HEAD ) );
  return this->value<std::string> ( path, Usul::System::Host::name() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the machine that is the head node.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::headNodeMachineName ( const std::string &head )
{
  _database[Sections::PREFERENCES][Keys::MACHINE][Keys::HEAD] = head;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the global-normalization flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::normalizeVertexNormalsGlobal() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::NORMALIZE, "/", Keys::GLOBAL ) );
  return this->value<bool> ( path, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the global-normalization flag.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::normalizeVertexNormalsGlobal ( bool state )
{
  _database[Sections::PREFERENCES][Keys::NORMALIZE][Keys::GLOBAL] = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the model-normalization flag.
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::normalizeVertexNormalsModels() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::NORMALIZE, "/", Keys::MODELS ) );
  return this->value<bool> ( path, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model-normalization flag.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::normalizeVertexNormalsModels ( bool state )
{
  _database[Sections::PREFERENCES][Keys::NORMALIZE][Keys::MODELS] = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu matrix.
//
///////////////////////////////////////////////////////////////////////////////

const Settings::Matrix Settings::menuMatrix() const
{
  std::string path0 ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::MENU, "/", Keys::SCALE ) );
  Usul::Math::Vec3f scale (  this->value<Usul::Math::Vec3f> ( path0, Usul::Math::Vec3f ( 0.0, 0.0, 0.0 ) ) );

  std::string path1 ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::MENU, "/", Keys::TRANSLATE ) );
  Usul::Math::Vec3f translate (  this->value<Usul::Math::Vec3f> ( path1, Usul::Math::Vec3f ( 0.0, 0.0, 0.0 ) ) );

  return Matrix::translation ( translate ) * Matrix::scale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu matrix.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Matrix Settings::menuMatrix()
{
  Vec3f scale     ( _database[Sections::PREFERENCES][Keys::MENU][Keys::SCALE    ].get<Vec3f> ( Vec3f ( 0.0, 0.0, 0.0 ) ) );
  Vec3f translate ( _database[Sections::PREFERENCES][Keys::MENU][Keys::TRANSLATE].get<Vec3f> ( Vec3f ( 0.0, 0.0, 0.0 ) ) );

  return Matrix::translation ( translate ) * Matrix::scale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuMatrix ( const Matrix &m )
{
  Usul::Math::Vec3f scale ( m[Matrix::SCALE_X], m[Matrix::SCALE_Y], m[Matrix::SCALE_Z] );
  Usul::Math::Vec3f translate ( m[Matrix::TRANSLATION_X], m[Matrix::TRANSLATION_Y], m[Matrix::TRANSLATION_Z] );

  _database[Sections::PREFERENCES][Keys::MENU][Keys::SCALE] = scale;
  _database[Sections::PREFERENCES][Keys::MENU][Keys::TRANSLATE] = translate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu background normal color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuBgColorNorm  () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::BACKGROUND, "/",
                                             Keys::NORMAL ) );
  return this->value<Color> ( path, Color ( 0.6f, 0.6f, 0.6f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu background normal color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuBgColorNorm  ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::BACKGROUND][Keys::NORMAL] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu background highlighted color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuBgColorHLght () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::BACKGROUND, "/",
                                             Keys::HIGHLIGHT ) );
  return this->value<Color> ( path, Color ( 0.4f, 0.4f, 0.4f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu background highlighted color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuBgColorHLght ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::BACKGROUND][Keys::HIGHLIGHT] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu background disabled color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuBgColorDsabl () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::BACKGROUND, "/",
                                             Keys::DISABLED ) );
  return this->value<Color> ( path, Color ( 0.6f, 0.6f, 0.6f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu background disabled color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuBgColorDsabl ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::BACKGROUND][Keys::DISABLED] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text normal color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuTxtColorNorm  () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::TEXT_KEY, "/",
                                             Keys::NORMAL ) );
  return this->value<Color> ( path, Color ( 0.0f, 0.0f, 0.0f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu text normal color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuTxtColorNorm  ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::TEXT_KEY][Keys::NORMAL] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text highlighted color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuTxtColorHLght () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::TEXT_KEY, "/",
                                             Keys::HIGHLIGHT ) );
  return this->value<Color> ( path, Color ( 0.0f, 0.0f, 0.0f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu text highlighted color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuTxtColorHLght ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::TEXT_KEY][Keys::HIGHLIGHT] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the menu text disabled color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::menuTxtColorDsabl () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::MENU, "/", 
                                             Keys::COLOR, "/",
                                             Keys::TEXT_KEY, "/",
                                             Keys::DISABLED ) );
  return this->value<Color> ( path, Color ( 0.3f, 0.3f, 0.3f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the menu text disabled color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::menuTxtColorDsabl ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::MENU][Keys::COLOR][Keys::TEXT_KEY][Keys::DISABLED] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar matrix.
//
///////////////////////////////////////////////////////////////////////////////

const Settings::Matrix Settings::statusBarMatrix() const
{
  std::string path0 ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::STATUS_BAR, "/", Keys::SCALE ) );
  Usul::Math::Vec3f scale (  this->value<Usul::Math::Vec3f> ( path0, Usul::Math::Vec3f ( 0.0, 0.0, 0.0 ) ) );

  std::string path1 ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::STATUS_BAR, "/", Keys::TRANSLATE ) );
  Usul::Math::Vec3f translate (  this->value<Usul::Math::Vec3f> ( path1, Usul::Math::Vec3f ( 0.0, 0.0, 0.0 ) ) );

  return Matrix::translation ( translate ) * Matrix::scale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status-bar matrix.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Matrix Settings::statusBarMatrix()
{
  Vec3f scale     ( _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::SCALE    ].get<Vec3f> ( Vec3f ( 0.0, 0.0, 0.0 ) ) );
  Vec3f translate ( _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::TRANSLATE].get<Vec3f> ( Vec3f ( 0.0, 0.0, 0.0 ) ) );

  return Matrix::translation ( translate ) * Matrix::scale ( scale );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status-bar matrix.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::statusBarMatrix ( const Matrix &m )
{
  Usul::Math::Vec3f scale ( m[Matrix::SCALE_X], m[Matrix::SCALE_Y], m[Matrix::SCALE_Z] );
  Usul::Math::Vec3f translate ( m[Matrix::TRANSLATION_X], m[Matrix::TRANSLATION_Y], m[Matrix::TRANSLATION_Z] );

  _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::SCALE] = scale;
  _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::TRANSLATE] = translate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the flag that says whether or not the status-bar is visible at startup.
//
///////////////////////////////////////////////////////////////////////////////

bool Settings::statusBarVisibleAtStartup() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::STATUS_BAR, "/", Keys::VISIBLE ) );
  return this->value<bool> ( path, false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag that says whether or not the status-bar is visible at startup.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::statusBarVisibleAtStartup ( bool state )
{
  _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::VISIBLE] = state;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status bar background color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::statusBgColor () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::STATUS_BAR, "/", 
                                             Keys::COLOR, "/",
                                             Keys::BACKGROUND ) );
  return this->value<Color> ( path, Color ( 0.6f, 0.6f, 0.6f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar background color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::statusBgColor ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::COLOR][Keys::BACKGROUND] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the status bar text color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::statusTxtColor () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", 
                                             Keys::STATUS_BAR, "/", 
                                             Keys::COLOR, "/",
                                             Keys::TEXT_KEY ) );
  return this->value<Color> ( path, Color ( 0.0f, 0.0f, 0.0f, 1.0f ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the status bar text color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::statusTxtColor ( const Color &c )
{
  _database[Sections::PREFERENCES][Keys::STATUS_BAR][Keys::COLOR][Keys::TEXT_KEY] = c;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the relative translation speed.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::translationSpeed() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::SPEED, "/", Keys::TRANSLATE ) );
  return this->value<float> ( path, 0.05f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the relative translation speed.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::translationSpeed ( float s )
{
  _database[Sections::PREFERENCES][Keys::SPEED][Keys::TRANSLATE] = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the rotation speed.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::rotationSpeed() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::SPEED, "/", Keys::ROTATE ) );
  return this->value<float> ( path, 30.0f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation speed.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::rotationSpeed ( float s )
{
  _database[Sections::PREFERENCES][Keys::SPEED][Keys::ROTATE] = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale-speed.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::scaleSpeed() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::SPEED, "/", Keys::SCALE ) );
  return this->value<float> ( path, 0.75f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the scale-speed.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::scaleSpeed ( float s )
{
  _database[Sections::PREFERENCES][Keys::SPEED][Keys::SCALE] = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the selection color.
//
///////////////////////////////////////////////////////////////////////////////

Settings::Color Settings::selectionColor() const
{
  return _selectColor;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the selection color.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::selectionColor ( const Color &c )
{
  _selectColor = c;
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Set the frame scale multiplier.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::frameScale ( float f )
{
  _database[Sections::PREFERENCES][Keys::IMAGE][Keys::FRAME_SCALE] = f;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the frame scale multiplier.
//
///////////////////////////////////////////////////////////////////////////////

float Settings::frameScale () const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::IMAGE, "/", Keys::FRAME_SCALE ) );
  return this->value<float> ( path, 1.0f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image export extension.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::imageExportExtension ( const std::string& ext )
{
  _database[Sections::PREFERENCES][Keys::IMAGE][Keys::EXTENSION] = ext;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the image export extension.
//
///////////////////////////////////////////////////////////////////////////////

std::string Settings::imageExportExtension ( ) const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::IMAGE, "/", Keys::EXTENSION ) );
  return this->value<std::string> ( path, ".jpg" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image directory.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::imageDirectory ( const std::string& s )
{
  _database[Sections::PREFERENCES][Keys::IMAGE][Keys::DIRECTORY] = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the image directory.
//
///////////////////////////////////////////////////////////////////////////////

std::string Settings::imageDirectory() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::IMAGE, "/", Keys::DIRECTORY ) );
  return this->value<std::string> ( path, Usul::File::Temp::directory ( true ) );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Set the wand offset.
//
///////////////////////////////////////////////////////////////////////////////

void Settings::wandOffset ( const Usul::Math::Vec3d& o )
{
  _database[Sections::PREFERENCES][Keys::WAND_OFFSET] = o;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the wand offset.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Settings::wandOffset() const
{
  std::string path ( Usul::Strings::format ( Sections::PREFERENCES, "/", Keys::WAND_OFFSET ) );
  return this->value<Vec3d> ( path, Vec3d ( 0.0, 0.0, 0.0 ) );
}
