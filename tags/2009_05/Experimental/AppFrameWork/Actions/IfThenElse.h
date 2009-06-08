
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Conditional update.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_IF_THEN_ELSE_UPDATE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_IF_THEN_ELSE_UPDATE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"
#include "AppFrameWork/Conditions/Condition.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT IfThenElse : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( IfThenElse );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( IfThenElse );

  // Constructor
  IfThenElse ( AFW::Conditions::Condition *check, AFW::Actions::UpdateAction *update, AFW::Actions::UpdateAction *else_ = 0x0 );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~IfThenElse();

private:

  // No copying.
  IfThenElse ( const IfThenElse & );
  IfThenElse &operator = ( const IfThenElse & );

  AFW::Conditions::Condition::RefPtr _check;
  AFW::Actions::UpdateAction::RefPtr _update;
  AFW::Actions::UpdateAction::RefPtr _else;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_IF_THEN_ELSE_UPDATE_ACTION_CLASS_H_
