
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Error-checking functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_ERROR_CHECKING_
#define _MENUKIT_ERROR_CHECKING_

#include "MenuKit/Export.h"


namespace MenuKit {

class Item;

struct MENUKIT_EXPORT Errors
{
  // Throws if the pointer is null.
  static void checkPointer ( unsigned int id, const void *ptr );

  // Throws if the state is false.
  static void checkState ( unsigned int id, bool state );

  // Throws if the parent is not null.
  static void ensureNullParent ( unsigned int id, const Item *item );
};


}; // namespace MenuKit


#define MENUKIT_CHECK_POINTER       MenuKit::Errors::checkPointer
#define MENUKIT_CHECK_STATE         MenuKit::Errors::checkState
#define MENUKIT_ENSURE_NULL_PARENT  MenuKit::Errors::ensureNullParent


#endif // _MENUKIT_ERROR_CHECKING_
