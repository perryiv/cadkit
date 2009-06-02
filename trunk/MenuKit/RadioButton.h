
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MENU_KIT_RADIO_BUTTON_H__
#define __MENU_KIT_RADIO_BUTTON_H__

#include "MenuKit/Button.h"

#include "Usul/Commands/GenericCheckCommand.h"

namespace MenuKit {


class MENUKIT_EXPORT RadioButton : public Button
{
public:

  // Typedefs and smart-pointers.
  typedef Button BaseClass;
  typedef Usul::Commands::GenericCheckCommand CheckCommand;
  typedef CheckCommand::ExecuteFunctor ExecuteFunction;
  typedef CheckCommand::CheckFunctor CheckFunction;
  typedef CheckCommand::EnableFunctor EnableFunction;
  USUL_DECLARE_REF_POINTERS ( RadioButton );

  // Construction.
  RadioButton();
  RadioButton ( Usul::Commands::Command * );
  RadioButton ( const RadioButton & );

  // Assignment.
  RadioButton& operator = ( const RadioButton & );

  static RadioButton* create ( const std::string &name, ExecuteFunction f, CheckFunction c );
  static RadioButton* create ( const std::string &name, ExecuteFunction f, CheckFunction c, EnableFunction e );

  // Accept the visitor.
  virtual void        accept ( Visitor &v );

protected:

  // Use reference counting.
  virtual ~RadioButton();
};


} // namespace MenuKit

#endif // __MENU_KIT_RADIO_BUTTON_H__
