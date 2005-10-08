
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Separator class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_SEPARATOR_CLASS_H_
#define _FOX_TOOLS_MENU_SEPARATOR_CLASS_H_

#include "FoxTools/Items/Command.h"

namespace FX { class FXMenuSeparator; }


namespace FoxTools {
namespace Menu {

class Group;

class FOX_TOOLS_EXPORT Separator : public FoxTools::Items::Command
{
public:

  // Typedefs.
  typedef FoxTools::Items::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Separator );

  // Constructors.
  Separator ( unsigned int token = 0 );

  // Clear the button.
  virtual void          clear();

protected:

  // Use reference counting.
  virtual ~Separator();

  // Build the button.
  virtual void          _build ( FX::FXComposite *parent );

  // Create
  virtual void          _create ();

private:

  // No copying.
  Separator ( const Separator & );
  Separator &operator = ( const Separator & );

  friend class Group;

  FX::FXMenuSeparator *_separator;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_SEPARATOR_CLASS_H_
