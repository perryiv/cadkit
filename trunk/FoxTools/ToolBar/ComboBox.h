
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

#ifndef _FOX_TOOLS_TOOLBAR_COMBO_BOX_H_
#define _FOX_TOOLS_TOOLBAR_COMBO_BOX_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Items/Command.h"

#include <string>

namespace FX { class FXComboBox; }


namespace FoxTools {
namespace ToolBar {

class Bar;

class FOX_TOOLS_EXPORT ComboBox : public FoxTools::Items::Command
{
public:
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ComboBox );

  ComboBox ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0  );

  // Clear the button
  virtual void      clear();

  // Get the combo box
  FX::FXComboBox*   comboBox() { return _comboBox; }

protected:

  // Use reference counting.
  virtual ~ComboBox();

  virtual void          _build ( FX::FXComposite *parent );

private:

  friend class Bar;

  // No copying.
  ComboBox ( const ComboBox & );
  ComboBox &operator = ( const ComboBox & );

  FX::FXComboBox *_comboBox;

};


} // namespace ToolBar
} // namespace FoxTools


#endif // _FOX_TOOLS_TOOLBAR_COMBO_BOX_H_

