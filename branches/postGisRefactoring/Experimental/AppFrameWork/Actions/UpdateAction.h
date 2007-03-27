
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all update actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_UPDATE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_UPDATE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/Action.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT UpdateAction : public Action
{
public:

  // Typedefs.
  typedef Action BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( UpdateAction );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( UpdateAction );

protected:

  // Constructor
  UpdateAction();

  // Use reference counting.
  virtual ~UpdateAction();

private:

  // No copying.
  UpdateAction ( const UpdateAction & );
  UpdateAction &operator = ( const UpdateAction & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_UPDATE_ACTION_CLASS_H_
