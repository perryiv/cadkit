
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for opening an existing model.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_
#define _APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT OpenModelAction : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( OpenModelAction );

  // Constructor
  OpenModelAction();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0, Usul::Base::Referenced *data = 0x0 );

protected:

  // Use reference counting.
  virtual ~OpenModelAction();

private:

  // No copying.
  OpenModelAction ( const OpenModelAction & );
  OpenModelAction &operator = ( const OpenModelAction & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_OPEN_MODEL_ACTION_CLASS_H_
