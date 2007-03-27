
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for opening a new model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_NEW_MODEL_ACTION_CLASS_H_
#define _APP_FRAME_WORK_NEW_MODEL_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT NewModelAction : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( NewModelAction );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( NewModelAction );

  // Constructor
  NewModelAction();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~NewModelAction();

private:

  // No copying.
  NewModelAction ( const NewModelAction & );
  NewModelAction &operator = ( const NewModelAction & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_NEW_MODEL_ACTION_CLASS_H_
