
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_KIT_TOGGLE_BUTTON_H__
#define __MENU_KIT_TOGGLE_BUTTON_H__

#include "MenuKit/Button.h"

#include "boost/function.hpp"

namespace MenuKit {


class MENUKIT_EXPORT ToggleButton : public Button
{
public:

  // Typedefs and smart-pointers.
  typedef Button BaseClass;
  typedef boost::function<void ( bool )> ExecuteFunction;
  typedef boost::function<bool ()> EnableFunction;
  typedef boost::function<bool ()> CheckFunction;
  USUL_DECLARE_REF_POINTERS ( ToggleButton );

  // Construction.
  ToggleButton ();
  ToggleButton ( Usul::Commands::Command * );
  ToggleButton ( const ToggleButton & );

  // Assignment.
  ToggleButton& operator = ( const ToggleButton & );

  static ToggleButton* create ( const std::string &name, ExecuteFunction f, CheckFunction c );

  // Accept the visitor.
  virtual void        accept ( Visitor &v );

protected:

  // Use reference counting.
  virtual ~ToggleButton();
};


} // namespace MenuKit


#endif // __MENU_KIT_TOGGLE_BUTTON_H__
