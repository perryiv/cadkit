
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base command class for menus.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_COMMAND_CLASS_H_
#define _FOX_TOOLS_MENU_COMMAND_CLASS_H_

#include "FoxTools/Items/Item.h"

namespace FX { class FXObject; class FXComposite; }


namespace FoxTools {

  namespace Menu { class Group; }
  namespace ToolBar { class Bar; }

namespace Items {


class FOX_TOOLS_EXPORT Command : public FoxTools::Items::Item
{
public:

  // Typedefs.
  typedef FoxTools::Items::Item BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Command );

  // Set/get the description.
  const std::string &   description() const;
  void                  description ( const std::string & );

  // Set/get the hot-keys.
  const std::string &   hotKeys() const;
  void                  hotKeys ( const std::string & );

  // Set/get the target.
  FX::FXObject *        target();
  const FX::FXObject *  target() const;
  void                  target ( FX::FXObject * );

  // Set/get the token.
  unsigned int          token() const;
  void                  token ( unsigned int );

  // Set/get the selector.
  unsigned int          selector() const;
  void                  selector ( unsigned int );

protected:

  // Constructors.
  Command ( unsigned int token = 0 );
  Command ( const std::string &name, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );
  Command ( const std::string &name, const std::string &hotKeys, const std::string &description, FX::FXObject *target, unsigned int selector, unsigned int token = 0 );

  // Use reference counting.
  virtual ~Command();

  // Build the command.
  virtual void          _build ( FX::FXComposite *parent ) = 0;

private:

  // No copying.
  Command ( const Command & );
  Command &operator = ( const Command & );

  friend class FoxTools::Menu::Group;
  friend class FoxTools::ToolBar::Bar;

  unsigned int _token;
  std::string _hotKeys;
  std::string _description;
  FX::FXObject *_target;
  unsigned int _selector;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_COMMAND_CLASS_H_
