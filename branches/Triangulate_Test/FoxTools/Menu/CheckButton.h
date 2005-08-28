
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check button class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_CHECK_BUTTON_CLASS_H_
#define _FOX_TOOLS_MENU_CHECK_BUTTON_CLASS_H_

#include "FoxTools/Menu/Button.h"


namespace FoxTools {
namespace Menu {

class Group;

class FOX_TOOLS_EXPORT CheckButton : public FoxTools::Menu::Button
{
public:

  // Typedefs.
  typedef FoxTools::Menu::Button BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CheckButton );

  // Constructors.
  CheckButton ( unsigned int token = 0 );
  CheckButton ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );
  CheckButton ( const std::string &name, const std::string &hotKeys, const std::string &description, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );

protected:

  // Use reference counting.
  virtual ~CheckButton();

  // Build the button.
  virtual void          _build ( FX::FXComposite *parent );

private:

  // No copying.
  CheckButton ( const CheckButton & );
  CheckButton &operator = ( const CheckButton & );
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_CHECK_BUTTON_CLASS_H_
