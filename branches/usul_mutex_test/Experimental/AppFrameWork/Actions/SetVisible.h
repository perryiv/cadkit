
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the visible state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_SET_VISIBLE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_SET_VISIBLE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT SetVisible : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SetVisible );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( SetVisible );

  // Constructors
  SetVisible ( bool state );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~SetVisible();

private:

  // No copying.
  SetVisible ( const SetVisible & );
  SetVisible &operator = ( const SetVisible & );

  bool _state;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_SET_VISIBLE_ACTION_CLASS_H_
