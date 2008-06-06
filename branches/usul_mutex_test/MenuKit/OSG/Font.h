
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  For working with fonts.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_OSG_FONT_H_
#define _MENUKIT_OSG_FONT_H_

#include "MenuKit/OSG/Export.h"

#include <string>


namespace MenuKit {
namespace OSG {


struct MENUKIT_OSG_EXPORT Font
{
  //  Return the font filename.
  static std::string         filename ( const std::string &fontName );
};


}; // namespace OSG
}; // namespace MenuKit


#endif // _MENUKIT_OSG_FONT_H_
