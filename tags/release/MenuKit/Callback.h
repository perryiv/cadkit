
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callbacks class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_CALLBACK_H_
#define _MENUKIT_CALLBACK_H_

#include "MenuKit/Referenced.h"
#include "MenuKit/Messages.h"


namespace MenuKit {

class Item;

class MENUKIT_EXPORT Callback : public Referenced
{
public:

  // Typedefs and smart-pointers.
  typedef Referenced BaseClass;
  MENUKIT_DECLARE_POINTER ( Callback );

  // Execute the callback functionality.
  virtual void operator () ( MenuKit::Message m, Item *item ) = 0;

protected:

  // Construction.
  Callback();
  Callback ( const Callback & );
  Callback& operator = ( const Callback & );

  // Use reference counting.
  virtual ~Callback();
};


}; // namespace MenuKit


#endif // _MENUKIT_CALLBACK_H_
