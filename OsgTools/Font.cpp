
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

#include "OsgTools/Font.h"

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Return the font filename.
//
///////////////////////////////////////////////////////////////////////////////

std::string Font::filename ( const std::string &name )
{
#ifdef _WIN32
  return name + ".ttf";
#else
  return std::string ( "fonts/" ) + name + std::string ( ".ttf" );
#endif
}
