
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Cursor factory class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_CURSOR_FACTORY_H_
#define _FOX_TOOLS_CURSOR_FACTORY_H_

#include "FoxTools/Export/Export.h"

namespace FX { class FXCursor; };

namespace FoxTools {
namespace Cursors {


class FOX_TOOLS_EXPORT Factory
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

  // Create a cursor. Calls "make" then FX::FXCursor::create().
  static FX::FXCursor *       create ( Type type );

  // Make the cursor. Does not create.
  static FX::FXCursor *       make ( Type type );

protected:

};


}; // namespace Cursors
}; // namespace FoxTools


#endif // _FOX_TOOLS_CURSOR_FACTORY_H_
