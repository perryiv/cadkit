
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Enable the control.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ENABLE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_ENABLE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT Enable : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Enable );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Enable );

  // Constructor
  Enable ( bool state = true );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~Enable();

private:

  // No copying.
  Enable ( const Enable & );
  Enable &operator = ( const Enable & );

  bool _state;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_ENABLE_ACTION_CLASS_H_
