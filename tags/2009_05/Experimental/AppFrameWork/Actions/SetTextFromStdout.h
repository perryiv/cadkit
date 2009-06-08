
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Set the window's text from stdout.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_SET_TEXT_FROM_STDOUT_ACTION_CLASS_H_
#define _APP_FRAME_WORK_SET_TEXT_FROM_STDOUT_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT SetTextFromStdout : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( SetTextFromStdout );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( SetTextFromStdout );

  // Constructor
  SetTextFromStdout();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~SetTextFromStdout();

private:

  // No copying.
  SetTextFromStdout ( const SetTextFromStdout & );
  SetTextFromStdout &operator = ( const SetTextFromStdout & );

  unsigned int _start;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_SET_TEXT_FROM_STDOUT_ACTION_CLASS_H_
