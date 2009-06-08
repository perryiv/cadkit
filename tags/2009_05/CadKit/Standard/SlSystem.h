
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlSystem: Info about the system.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef _CADKIT_STANDARD_LIBRARY_SYSTEM_INFO_CLASS_
#define _CADKIT_STANDARD_LIBRARY_SYSTEM_INFO_CLASS_


namespace CadKit
{
class SlSystem
{
public:

  // Get the number of mouse buttons.
  static unsigned int           getNumMouseButtons();

  // If there a mouse wheel?
  static bool                   hasMouseWheel();
};


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_SYSTEM_INFO_CLASS_
