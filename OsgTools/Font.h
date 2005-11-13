
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

#ifndef _OSG_TOOLS_FONT_H_
#define _OSG_TOOLS_FONT_H_

#include "OsgTools/Export.h"
#include "osgText/Font"


#include <string>


namespace OsgTools {


struct OSG_TOOLS_EXPORT Font
{
  //  Return the font filename.
  static std::string         fontfile ( const std::string &fontName );
  static osgText::Font*      defaultFont();
  
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_FONT_H_
