
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Enable the control if the condition is met.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ENABLE_IF_ACTION_CLASS_H_
#define _APP_FRAME_WORK_ENABLE_IF_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"

#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT EnableIf : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( EnableIf );

  // Constructor
  EnableIf ( UpdateAction *condition );

  // Get the proper state.
  virtual bool                        state();

protected:

  // Use reference counting.
  virtual ~EnableIf();

private:

  // No copying.
  EnableIf ( const EnableIf & );
  EnableIf &operator = ( const EnableIf & );

  UpdateAction::ValidRefPtr _condition;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_ENABLE_IF_ACTION_CLASS_H_
