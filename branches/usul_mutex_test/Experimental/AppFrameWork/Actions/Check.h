
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Check the control.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_CHECK_ACTION_CLASS_H_
#define _APP_FRAME_WORK_CHECK_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT Check : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Check );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Check );

  // Constructor
  Check ( AFW::Core::Object *, bool state = true );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~Check();

private:

  // No copying.
  Check ( const Check & );
  Check &operator = ( const Check & );

  bool _state;
  AFW::Core::Object::RefPtr _object;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_CHECK_ACTION_CLASS_H_
