
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

#include "Precompiled.h"
#include "Parser.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/Thrower.h"

#include "XmlTools/Reader.h"
#include "XmlTools/Policy.h"

#include <stdexcept>

using namespace VRV;
using namespace VRV::Prefs;


//  Constructor.
Parser::Parser() : 
  _settings ( 0x0 )
{
}


//  Destructor.
Parser::~Parser()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the user-preferences file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::read ( const std::string &filename, Settings *settings )
{
  // Reset.
  _settings = settings;

  try
  {
    // Read the file.
    this->_read ( filename );
  }

  // Catch exceptions.
  catch ( const std::runtime_error &e )
  {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1252925174, exception caught while reading user-preference file.",
          "\n\tFile: ", filename,
          "\n\tWhat: ", e.what() );
  }

  // Catch exceptions.
  catch ( ... )
  {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 1564710109, unknown exception caught while reading user-preference file.",
          "\n\tFile: ", filename );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to add multiple root nodes. This is to support legacy 
//  "CadViewer" files.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template
  <
    class XmlReaderType, 
    class KindOfCallback, 
    class TheCallbackType
  > 
  inline void add ( XmlReaderType &reader,   const KindOfCallback &kind, 
                    const std::string &path, const TheCallbackType &callback )
  {
    reader.add ( kind, std::string ( "cv/preferences/" )  + path, callback );
    reader.add ( kind, std::string ( "vrv/preferences/" ) + path, callback );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read the data from file.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_read ( const std::string &filename )
{
  // Define the xml-reader type.
  typedef void (Parser::*Function) ( const std::string &s );
  typedef Usul::Adaptors::MemberFunction < Parser*, Function > MemFun;
  typedef Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorPolicy;
  typedef XML::Reader < MemFun, XML::ValueCB, ErrorPolicy > XmlReader;

  // Make some member function adaptors.
  MemFun setNumGridBlocks  ( this, &Parser::_setNumGridBlocks  );
  MemFun setGridScale      ( this, &Parser::_setGridScale      );
  MemFun setGridColor      ( this, &Parser::_setGridColor      );
  MemFun setNearClip       ( this, &Parser::_setNearClip       );
  MemFun setViewAllScale   ( this, &Parser::_setViewAllScale   );
  MemFun setLightAmbient   ( this, &Parser::_setLightAmbient   );
  MemFun setLightDiffuse   ( this, &Parser::_setLightDiffuse   );
  MemFun setLightSpecular  ( this, &Parser::_setLightSpecular  );
  MemFun setLightDirection ( this, &Parser::_setLightDirection );
  MemFun setLightPosition  ( this, &Parser::_setLightPosition  );
  MemFun setBackground     ( this, &Parser::_setBackground     );
  MemFun setWriter         ( this, &Parser::_setWriter         );
  MemFun setNormGlobal     ( this, &Parser::_setNormGlobal     );
  MemFun setNormModels     ( this, &Parser::_setNormModels     );
  MemFun translateMenu     ( this, &Parser::_translateMenu     );
  MemFun scaleMenu         ( this, &Parser::_scaleMenu         );
  MemFun setMenuBgColorNorm   ( this, &Parser::_setMenuBgColorNorm   );
  MemFun setMenuBgColorHLght  ( this, &Parser::_setMenuBgColorHLght  );
  MemFun setMenuBgColorDsabl  ( this, &Parser::_setMenuBgColorDsabl  );
  MemFun setMenuTxtColorNorm  ( this, &Parser::_setMenuTxtColorNorm  );
  MemFun setMenuTxtColorHLght ( this, &Parser::_setMenuTxtColorHLght );
  MemFun setMenuTxtColorDsabl ( this, &Parser::_setMenuTxtColorDsabl );
  MemFun translateStatus   ( this, &Parser::_translateStatus   );
  MemFun scaleStatus       ( this, &Parser::_scaleStatus       );
  MemFun setStatusVisible  ( this, &Parser::_setStatusVisible  );
  MemFun setMenuHidesScene ( this, &Parser::_setMenuHidesScene );
  MemFun setTransSpeed     ( this, &Parser::_setTransSpeed     );
  MemFun setRotSpeed       ( this, &Parser::_setRotSpeed       );
  MemFun setScaleSpeed     ( this, &Parser::_setScaleSpeed     );

  // Declare the reader and add the callbacks.
  XmlReader reader;
  XmlReader::WhichCallback start ( XmlReader::NODE_START );
  Helper::add ( reader, start, "grid/num_blocks",          setNumGridBlocks  );
  Helper::add ( reader, start, "grid/scale",               setGridScale      );
  Helper::add ( reader, start, "grid/color",               setGridColor      );
  Helper::add ( reader, start, "clipping_plane/near",      setNearClip       );
  Helper::add ( reader, start, "view_all/scale",           setViewAllScale   );
  Helper::add ( reader, start, "light/ambient",            setLightAmbient   );
  Helper::add ( reader, start, "light/diffuse",            setLightDiffuse   );
  Helper::add ( reader, start, "light/specular",           setLightSpecular  );
  Helper::add ( reader, start, "light/direction",          setLightDirection );
  Helper::add ( reader, start, "light/position",           setLightPosition  );
  Helper::add ( reader, start, "background/color",         setBackground     );
  Helper::add ( reader, start, "machine/writer",           setWriter         );
  Helper::add ( reader, start, "normals/normalize/global", setNormGlobal     );
  Helper::add ( reader, start, "normals/normalize/models", setNormModels     );
  Helper::add ( reader, start, "menu/translate",           translateMenu     );
  Helper::add ( reader, start, "menu/scale",               scaleMenu         );
  Helper::add ( reader, start, "menu_color/bg_norm",       setMenuBgColorNorm   );
  Helper::add ( reader, start, "menu_color/bg_highlight",  setMenuBgColorHLght  );
  Helper::add ( reader, start, "menu_color/bg_disabled",   setMenuBgColorDsabl  );
  Helper::add ( reader, start, "menu_color/txt_norm",      setMenuTxtColorNorm  );
  Helper::add ( reader, start, "menu_color/txt_highlight", setMenuTxtColorHLght );
  Helper::add ( reader, start, "menu_color/txt_disabled",  setMenuTxtColorDsabl );
  Helper::add ( reader, start, "status_bar/translate",     translateStatus   );
  Helper::add ( reader, start, "status_bar/scale",         scaleStatus       );
  Helper::add ( reader, start, "status_bar/visible",       setStatusVisible  );
  Helper::add ( reader, start, "menu/hide_scene",          setMenuHidesScene );
  Helper::add ( reader, start, "speed/translate",          setTransSpeed     );
  Helper::add ( reader, start, "speed/rotate",             setRotSpeed       );
  Helper::add ( reader, start, "speed/scale",              setScaleSpeed     );

  // Read the file.
  try
  {
    reader.read ( filename );
  }

  // Catch exceptions from the reader and throw a better one.
  catch ( const XML::Error &e )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 4291010367, failed to read user-preferences file.",
        "\n\tFile:     ", filename,
        "\n\tLine:     ", e.line(),
        "\n\tPosition: ", e.position(),
        "\n\tMessage:  ", e.message() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string to a boolean.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  bool toBool ( const std::string &s )
  {
    std::string str ( s );
    std::transform ( str.begin(), str.end(), str.begin(), ::tolower );
    bool state ( ( "on" == str || "yes" == str || "true" == str || "1" == str ) ? true : false );
    return state;
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the string to a vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Vec_ > struct ToVec
  {
    typedef Vec_ Vec;
    static Vec convert ( const std::string &s )
    {
      Vec v;
      std::istringstream in ( s );

      for ( unsigned int i = 0; i < Vec::SIZE; ++i )
      {
        // Make sure there is one in the chamber.
        if ( in.fail() || in.eof() )
          {
// TODO: fix
//            Usul::Exceptions::Thrower<std::runtime_error>("Error 1083884154, corrupt vector in file.",
//                                                          "\n\tExpected Size: ", Vec::SIZE,
//                                                          "\n\tVector Read:   ", s );
          }

        // Read the value.
        in >> v[i];
      }

      // Return the vector.
      return v;
    }
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Set the number of grid blocks.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setNumGridBlocks ( const std::string &s )
{
  ErrorChecker ( 1083817587u, !s.empty() );
  _settings->numGridBlocks ( Helper::ToVec<Settings::Vec2ui>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the grid scale.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setGridScale ( const std::string &s )
{
  ErrorChecker ( 1083817588u, !s.empty() );
  _settings->gridScale ( Helper::ToVec<Settings::Vec2f>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the grid color.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setGridColor ( const std::string &s )
{
  ErrorChecker ( 1083817589u, !s.empty() );
  _settings->gridColor ( Helper::ToVec<Settings::Color>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the near clipping plane.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setNearClip ( const std::string &s )
{
  ErrorChecker ( 1083817590u, !s.empty() );
  _settings->nearClippingDistance ( float ( Usul::Math::abs ( ::atof ( s.c_str() ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the view-all scale factor.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setViewAllScale ( const std::string &s )
{
  ErrorChecker ( 1083817591u, !s.empty() );
  _settings->viewAllScaleZ ( float ( Usul::Math::abs ( ::atof ( s.c_str() ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light color.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLightAmbient ( const std::string &s )
{
  ErrorChecker ( 1083817582u, !s.empty() );
  _settings->ambientLightColor ( Helper::ToVec<Settings::Color>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light color.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLightDiffuse ( const std::string &s )
{
  ErrorChecker ( 1083817583u, !s.empty() );
  _settings->diffuseLightColor ( Helper::ToVec<Settings::Color>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light color.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLightSpecular( const std::string &s )
{
  ErrorChecker ( 1083817584u, !s.empty() );
  _settings->specularLightColor ( Helper::ToVec<Settings::Color>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light direction.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLightDirection ( const std::string &s )
{
  ErrorChecker ( 1083817585u, !s.empty() );
  _settings->lightDirection ( Helper::ToVec<Settings::Vec3f>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the light position.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setLightPosition ( const std::string &s )
{
  ErrorChecker ( 1083817594u, !s.empty() );
  _settings->lightPosition ( Helper::ToVec<Settings::Vec4f>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the background color.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setBackground ( const std::string &s )
{
  ErrorChecker ( 1083817586u, !s.empty() );
  _settings->backgroundColor ( Helper::ToVec<Settings::Color>::convert ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the machine that writes files.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setWriter ( const std::string &s )
{
  ErrorChecker ( 3769331235u, !s.empty() );
  _settings->fileWriterMachineName ( s );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the global GL_NORMALIZE flag.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setNormGlobal ( const std::string &s )
{
  ErrorChecker ( 3652665668u, !s.empty() );
  _settings->normalizeVertexNormalsGlobal ( Helper::toBool ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the global status-bar visibility flag.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setStatusVisible ( const std::string &s )
{
  ErrorChecker ( 1222882380u, !s.empty() );
  _settings->statusBarVisibleAtStartup ( Helper::toBool ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the model's GL_NORMALIZE flag.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setNormModels ( const std::string &s )
{
  ErrorChecker ( 1227435267u, !s.empty() );
  _settings->normalizeVertexNormalsModels ( Helper::toBool ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the flag for hiding the scene when the menu is showing.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuHidesScene ( const std::string &s )
{
  ErrorChecker ( 1083883561u, !s.empty() );
  _settings->menuHidesScene ( Helper::toBool ( s ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Translate the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_translateMenu ( const std::string &s )
{
  ErrorChecker ( 1083884664u, !s.empty() );
  Settings::Matrix trans;
  trans.makeTranslation ( Helper::ToVec<Settings::Vec3f>::convert ( s ) );
  const Settings::Matrix &current = _settings->menuMatrix();
  _settings->menuMatrix ( trans * current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale the menu.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_scaleMenu ( const std::string &s )
{
  ErrorChecker ( 1083888885u, !s.empty() );
  Settings::Matrix scale;
  scale.makeScale ( Helper::ToVec<Settings::Vec3f>::convert ( s ) );
  const Settings::Matrix &current = _settings->menuMatrix();
  _settings->menuMatrix ( scale * current );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's NORMAL background color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuBgColorNorm ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuBgColorNorm ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's HIGHLIGHTED background color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuBgColorHLght ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuBgColorHLght ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's DISABLED background color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuBgColorDsabl ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuBgColorDsabl ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's NORMAL text color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuTxtColorNorm ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuTxtColorNorm ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's HIGHLIGHTED text color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuTxtColorHLght ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuTxtColorHLght ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Set the Menu's DISABLED text color
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setMenuTxtColorDsabl ( const std::string &s )
{
  //ErrorChecker ( 1083817586u, !s.empty() );
  _settings->menuTxtColorDsabl ( Helper::ToVec<Settings::Color>::convert ( s ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Translate the status-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_translateStatus ( const std::string &s )
{
  ErrorChecker ( 3062056743u, !s.empty() );
  Settings::Matrix trans;
  trans.makeTranslation ( Helper::ToVec<Settings::Vec3f>::convert ( s ) );
  const Settings::Matrix &current = _settings->statusBarMatrix();
  _settings->statusBarMatrix ( trans * current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scale the status-bar.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_scaleStatus ( const std::string &s )
{
  ErrorChecker ( 2589903318u, !s.empty() );
  Settings::Matrix scale;
  scale.makeScale ( Helper::ToVec<Settings::Vec3f>::convert ( s ) );
  const Settings::Matrix &current = _settings->statusBarMatrix();
  _settings->statusBarMatrix ( scale * current );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the translation speed.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setTransSpeed ( const std::string &s )
{
  ErrorChecker ( 1083817590u, !s.empty() );
  _settings->translationSpeed ( float ( Usul::Math::abs ( ::atof ( s.c_str() ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the rotation speed.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setRotSpeed ( const std::string &s )
{
  ErrorChecker ( 1083817590u, !s.empty() );
  _settings->rotationSpeed ( float ( Usul::Math::abs ( ::atof ( s.c_str() ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the scale-speed.
//
///////////////////////////////////////////////////////////////////////////////

void Parser::_setScaleSpeed ( const std::string &s )
{
  ErrorChecker ( 2973371103u, !s.empty() );
  _settings->scaleSpeed ( float ( Usul::Math::abs ( ::atof ( s.c_str() ) ) ) );
}
