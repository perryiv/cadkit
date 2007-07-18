
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

#include "Settings.h"
#include "Parser.h"

#include "Usul/System/Host.h"

using namespace VRV;
using namespace VRV::Prefs;


// Constructor
Settings::Settings()  : BaseClass(),
                        _zNear          ( 0.1f ),
                        _zScale         ( 2 ),
                        _ambientLight   ( 0.2f, 0.2f, 0.2f, 1.0f ),
                        _diffuseLight   ( 1.0f, 1.0f, 1.0f, 1.0f ),
                        _specularLight  ( 0.7f, 0.7f, 0.7f, 1.0f ),
                        _lightDir       ( 0, 0, -1 ),
                        _lightPos       ( 1.0f , 25.0f, 10.0f, 1.0f ),
                        _background     ( 0, 0, 0, 1 ),
                        _writer         ( Usul::System::Host::name() ),
                        _normGlobal     ( true ),
                        _normModels     ( false ),
                        _menuMatrix     (),
                        _menuBgColorNorm   ( 0.6f, 0.6f, 0.6f, 1.0f ),
                        _menuBgColorHLght  ( 0.4f, 0.4f, 0.4f, 1.0f ),
                        _menuBgColorDsabl  ( 0.6f, 0.6f, 0.6f, 1.0f ),
                        _menuTxtColorNorm  ( 0.0f, 0.0f, 0.0f, 1.0f ),
                        _menuTxtColorHLght ( 0.0f, 0.0f, 0.0f, 1.0f ),
                        _menuTxtColorDsabl ( 0.3f, 0.3f, 0.3f, 1.0f ),
                        _statusMatrix   (),
                        _menuHidesScene ( false ),
                        _statusVisible  ( false ),
                        _statusBgColor  ( 0.6f, 0.6f, 0.6f, 1.0f ),
                        _statusTxtColor ( 0.0f, 0.0f, 0.0f, 1.0f ),
                        _transSpeed     ( 0.05f ), // percent / second
                        _rotSpeed       ( 30 )     // degrees / second
{
  _menuMatrix.identity();
  _statusMatrix.identity();
}


//  Destructor.
Settings::~Settings()
{
}


//  Read the user-settings file.
void Settings::read ( const std::string &filename )
{
  Parser parser;
  parser.read ( filename, this );
}
