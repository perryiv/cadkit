
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for creating the menu bar.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_CREATE_MENU_BAR_ACTION_CLASS_H_
#define _APP_FRAME_WORK_CREATE_MENU_BAR_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT CreateMenuBar : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CreateMenuBar );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( CreateMenuBar );

  // Constructor
  CreateMenuBar();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~CreateMenuBar();

private:

  // No copying.
  CreateMenuBar ( const CreateMenuBar & );
  CreateMenuBar &operator = ( const CreateMenuBar & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_CREATE_MENU_BAR_ACTION_CLASS_H_
