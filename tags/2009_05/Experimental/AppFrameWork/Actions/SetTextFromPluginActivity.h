
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from plugin activity.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_SET_TEXT_FROM_PLUGIN_ACTIVITY_ACTION_CLASS_H_
#define _APP_FRAME_WORK_SET_TEXT_FROM_PLUGIN_ACTIVITY_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT SetTextFromPluginActivity : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SetTextFromPluginActivity );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( SetTextFromPluginActivity );

  // Constructor
  SetTextFromPluginActivity();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~SetTextFromPluginActivity();

private:

  // No copying.
  SetTextFromPluginActivity ( const SetTextFromPluginActivity & );
  SetTextFromPluginActivity &operator = ( const SetTextFromPluginActivity & );

  unsigned int _size;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_SET_TEXT_FROM_PLUGIN_ACTIVITY_ACTION_CLASS_H_
