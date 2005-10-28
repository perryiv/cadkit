
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu commands.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_COMMAND_CLASS_H_
#define _APP_FRAME_WORK_MENU_COMMAND_CLASS_H_

#include "AppFrameWork/Menus/MenuItem.h"
#include "AppFrameWork/Actions/CommandAction.h"
#include "AppFrameWork/Actions/UpdateAction.h"

#include "Usul/Pointers/Pointers.h"

#include <vector>


namespace AFW {
namespace Menus {


class APP_FRAME_WORK_EXPORT MenuCommand : public MenuItem
{
public:

  // Typedefs.
  typedef MenuItem BaseClass;
  typedef std::vector < AFW::Actions::Action::ValidAccessRefPtr > CommandActions;
  typedef std::vector < AFW::Actions::UpdateAction::ValidAccessRefPtr > UpdateActions;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MenuCommand );

  // Access to the command actions.
  const CommandActions &              actions() const { return _actions; }
  CommandActions &                    actions()       { return _actions; }

  // Access to the command actions.
  const UpdateActions &               updateActions() const { return _updates; }
  UpdateActions &                     updateActions()       { return _updates; }

protected:

  // Constructor
  MenuCommand();
  MenuCommand ( const std::string &text, unsigned short underline = 0 );

  // Use reference counting.
  virtual ~MenuCommand();

private:

  // No copying.
  MenuCommand ( const MenuCommand & );
  MenuCommand &operator = ( const MenuCommand & );

  CommandActions _actions;
  UpdateActions _updates;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_COMMAND_CLASS_H_
