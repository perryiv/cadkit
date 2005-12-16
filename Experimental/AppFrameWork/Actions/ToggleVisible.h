
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Toggle the visible state.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_TOGGLE_VISIBLE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_TOGGLE_VISIBLE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT ToggleVisible : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ToggleVisible );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( ToggleVisible );

  // Constructors
  ToggleVisible();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~ToggleVisible();

private:

  // No copying.
  ToggleVisible ( const ToggleVisible & );
  ToggleVisible &operator = ( const ToggleVisible & );

  bool _state;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_TOGGLE_VISIBLE_ACTION_CLASS_H_
