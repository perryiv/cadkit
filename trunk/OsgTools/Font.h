
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

#include "Usul/Threads/Mutex.h"

#include "osg/ref_ptr"
#include "osgText/Font"

#include <string>

namespace OsgTools {

struct OSG_TOOLS_EXPORT Font
{
  typedef osg::ref_ptr<osgText::Font> FontPtr;

  //  Return the font filename.
  static std::string         fontfile ( const std::string &fontName );
  static FontPtr             defaultFont();

  static unsigned int        estimateTextWidth ( osgText::Text* text );
  
private:
  static Usul::Threads::Mutex _mutex;
  static FontPtr _font;
};

} // namespace OsgTools


#endif // _OSG_TOOLS_FONT_H_
