
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Radio button class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_RADIO_BUTTON_CLASS_H_
#define _FOX_TOOLS_MENU_RADIO_BUTTON_CLASS_H_

#include "FoxTools/Menu/Button.h"


namespace FoxTools {
namespace Menu {

class Group;

class FOX_TOOLS_EXPORT RadioButton : public FoxTools::Menu::Button
{
public:

  // Typedefs.
  typedef FoxTools::Menu::Button BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RadioButton );

  // Constructors.
  RadioButton ( unsigned int token = 0 );
  RadioButton ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );
  RadioButton ( const std::string &name, const std::string &hotKeys, const std::string &description, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );

  // Set the check.
  void                  check ( bool );

protected:

  // Use reference counting.
  virtual ~RadioButton();

  // Build the button.
  virtual void          _build ( FX::FXComposite *parent );

private:

  // No copying.
  RadioButton ( const RadioButton & );
  RadioButton &operator = ( const RadioButton & );
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_RADIO_BUTTON_CLASS_H_
