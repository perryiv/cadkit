
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for removing the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_REMOVE_MENU_BAR_ACTION_CLASS_H_
#define _APP_FRAME_WORK_REMOVE_MENU_BAR_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT RemoveMenuBar : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RemoveMenuBar );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( RemoveMenuBar );

  // Constructor
  RemoveMenuBar();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~RemoveMenuBar();

private:

  // No copying.
  RemoveMenuBar ( const RemoveMenuBar & );
  RemoveMenuBar &operator = ( const RemoveMenuBar & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_REMOVE_MENU_BAR_ACTION_CLASS_H_
