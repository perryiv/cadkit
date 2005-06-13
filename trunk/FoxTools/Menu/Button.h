
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Button class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_BUTTON_CLASS_H_
#define _FOX_TOOLS_MENU_BUTTON_CLASS_H_

#include "FoxTools/Items/Command.h"

namespace FX { class FXMenuCommand; }


namespace FoxTools {
namespace Menu {

class Group;

class FOX_TOOLS_EXPORT Button : public FoxTools::Items::Command
{
public:

  // Typedefs.
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Button );

  // Constructors.
  Button ( unsigned int token = 0 );
  Button ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );
  Button ( const std::string &name, const std::string &hotKeys, const std::string &description, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );

  // Clear the button.
  virtual void          clear();

  // Enable/disable the internal FOX window.
  void                  enable ( bool );

protected:

  // Use reference counting.
  virtual ~Button();

  virtual void          _build ( FX::FXComposite *parent );
  FX::FXMenuCommand *   _button();
  void                  _button ( FX::FXMenuCommand * );

private:

  // No copying.
  Button ( const Button & );
  Button &operator = ( const Button & );

  friend class Group;

  FX::FXMenuCommand *_command;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_BUTTON_CLASS_H_
