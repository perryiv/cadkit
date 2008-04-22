
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wait cursor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_WAIT_CURSOR_H_
#define _FOX_TOOLS_WAIT_CURSOR_H_

#include "FoxTools/Export/Export.h"


namespace FoxTools {
namespace Adaptors {

class FOX_TOOLS_EXPORT Wait
{
public:

  Wait();
  ~Wait();

private:

  Wait ( const Wait & );             // No copying
  Wait &operator = ( const Wait & ); // No assignment
};

}; //namespace Adaptors
}; // namespace FoxTools


#endif // _FOX_TOOLS_WAIT_CURSOR_H_
