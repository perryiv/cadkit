
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu button.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_BUTTON_
#define _MENUKIT_BUTTON_

#include "MenuKit/Item.h"


namespace MenuKit {

class Visitor;

class MENUKIT_EXPORT Button : public Item
{
public:

  // Typedefs and smart-pointers.
  typedef Item BaseClass;
  MENUKIT_DECLARE_POINTER ( Button );

  // Construction.
  Button();
  Button ( const std::string &name, const std::string &icon_file, Callback *cb, MenuKit::Message m );
  Button ( const std::string &name, const std::string &icon_file );
  Button ( const Button & );

  // Assignment.
  Button& operator = ( const Button & );

  // Accept the visitor.
  virtual void        accept ( Visitor &v );

protected:

  // Use reference counting.
  virtual ~Button();
};


}; // namespace MenuKit


#endif // _MENUKIT_BUTTON_
