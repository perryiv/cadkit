
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cursor class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_BINDING_CURSOR_H_
#define _FOX_BINDING_CURSOR_H_

#include "FoxTools/FoxToolsDll.h"

namespace FX { class FXApp; class FXCursor; }

namespace FoxTools {
namespace Images {

class FOX_TOOLS_EXPORT Cursor
{
public:

  enum Type // Standard types.
  {
    CURSOR_ARROW = 1,
    CURSOR_ROTATE,
    CURSOR_TRANSLATE,
    CURSOR_SCALE,
    CURSOR_SEEK,
    CURSOR_HYPERLINK,
    CURSOR_SELECTION,
    CURSOR_GUN_SIGHT
  };

  enum Format // Array format.
  {
    CURSOR_XBM = 1,
    CURSOR_CUR,
  };

  // Create a standard cursor.
  static FX::FXCursor *       create ( Type type, 
                                       bool createIt,
                                       FX::FXApp *app );

  // Create a cursor from the given source and mask arrays.
  static FX::FXCursor *       create ( const unsigned char *source, 
                                       const unsigned char *mask, 
                                       int width, 
                                       int height, 
                                       int hotSpotX, 
                                       int hotSpotY, 
                                       Format arrayFormat,
                                       bool createIt,
                                       FX::FXApp *app );
};

}; //namespace Tools

}; // namespace FoxTools


#endif
