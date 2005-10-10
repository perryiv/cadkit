
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Struct for working with fonts.
//
///////////////////////////////////////////////////////////////////////////////

#include "Font.h"
#include "osgText/Font"
#include <fstream>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Return the font filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string Font::fontfile ( const std::string &name )
{
#ifdef _WIN32
  return name + ".ttf";
#elif __APPLE__
  return std::string ( "/Library/Fonts/" ) + name + std::string ( ".ttf" ); 
#else
  return std::string ( "fonts/" ) + name + std::string ( ".ttf" );
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default font for the platform.
//
///////////////////////////////////////////////////////////////////////////////

osgText::Font* Font::defaultFont() 
{
  #ifdef _WIN32

     return osgText::readFontFile( "arial.ttf" );

  #elif __APPLE__

    std::vector< std::string > fonts;
    fonts.push_back("fonts/arial.ttf");
    fonts.push_back("fonts/fudd.ttf");
    fonts.push_back("/Library/Fonts/fudd.ttf");
    fonts.push_back("/Library/Fonts/Vera.ttf");
    fonts.push_back("~/Library/Fonts/fudd.ttf");
    fonts.push_back("~/Library/Fonts/Vera.ttf");
    
    for (int i=0;i<fonts.size();++i) 
    {
      std::ifstream file (fonts[i].c_str(), std::ios::in );
      if ( file.is_open() ) {
        osgText::Font* dFont = osgText::readFontFile( fonts[i]);
        if (dFont != 0x0 ) {
          return dFont;
        }
      }
    }
    return 0x0;  //This will cause OSG to load its internal font

  #else

    return osgText::readFontFile( "/usr/share/fonts/TrueType/ttf-bitstream/Ver/Vera.ttf" );

  #endif
}
