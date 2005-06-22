
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Toolbar Toggle Button
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_TOOLBAR_TOGGLE_BUTTON_H_
#define _FOX_TOOLS_TOOLBAR_TOGGLE_BUTTON_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Command.h"

#include <string>

namespace FX { class FXToggleButton; }


namespace FoxTools {
namespace ToolBar {

class Bar;

class FOX_TOOLS_EXPORT ToggleButton : public FoxTools::Items::Command
{
public:
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ToggleButton );

  ToggleButton ( const std::string &name, const std::string &description, unsigned int checkedIconId, unsigned int uncheckedIconId, FX::FXObject *target, unsigned int selector, unsigned int token = 0  );
  ToggleButton ( const std::string &name, const std::string &description, unsigned int iconId, FX::FXObject *target, unsigned int selector, unsigned int token = 0  );

  // Clear the button
  virtual void clear();

protected:

  // Use reference counting.
  virtual ~ToggleButton();

  virtual void          _build ( FX::FXComposite *parent );

private:

  friend class Bar;

  // No copying.
  ToggleButton ( const ToggleButton & );
  ToggleButton &operator = ( const ToggleButton & );

  FX::FXToggleButton *_button;
  unsigned int _checkedIconId;
  unsigned int _uncheckedIconId;
};


} // namespace ToolBar
} // namespace FoxTools


#endif // _FOX_TOOLS_TOOLBAR_TOGGLE_BUTTON_H_

