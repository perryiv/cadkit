
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

#ifndef _CADKIT_GUI_FOX_CURSOR_H_
#define _CADKIT_GUI_FOX_CURSOR_H_

#include "FoxApi.h"

namespace FX
{
  class FXCursor;
  class FXApp;
};


namespace CadKit
{
class GUI_FOX_API FoxCursor
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
  };

  enum Format // Array format.
  {
    CURSOR_XBM = 1,
    CURSOR_CUR,
  };

  // Create a standard cursor.
  static FX::FXCursor *       create ( const Type &type, 
                                       const bool &createIt,
                                       FX::FXApp *app );

  // Create a cursor from the given source and mask arrays.
  static FX::FXCursor *       create ( const unsigned char *source, 
                                       const unsigned char *mask, 
                                       const int &width, 
                                       const int &height, 
                                       const int &hotSpotX, 
                                       const int &hotSpotY, 
                                       const Format &arrayFormat,
                                       const bool &createIt,
                                       FX::FXApp *app );
};
};

#endif
