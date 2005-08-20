
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functions to work with menu buttons.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_
#define _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Command.h"

#include <string>

namespace FX { class FXButton; }


namespace FoxTools {
namespace ToolBar {


class Bar;

class FOX_TOOLS_EXPORT Button : public FoxTools::Items::Command
{
public:
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Button );

  Button ( const std::string &name, const std::string &description, unsigned int iconId, FX::FXObject *target, unsigned int selector, unsigned int token = 0  );

  // Clear the button
  virtual void clear();

protected:

  // Use reference counting.
  virtual ~Button();

  virtual void          _build ( FX::FXComposite *parent );

  // Create
  virtual void          _create ();

private:

  friend class Bar;

  // No copying.
  Button ( const Button & );
  Button &operator = ( const Button & );

  FX::FXButton *_button;
  unsigned int _iconId;
};


} // namespace ToolBar
} // namespace FoxTools


#endif // _FOX_TOOLS_MENU_BUTTON_FUNCTIONS_H_

