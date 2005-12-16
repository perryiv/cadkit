
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all command actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_COMMAND_ACTION_CLASS_H_
#define _APP_FRAME_WORK_COMMAND_ACTION_CLASS_H_

#include "AppFrameWork/Actions/Action.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT CommandAction : public Action
{
public:

  // Typedefs.
  typedef Action BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CommandAction );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( CommandAction );

protected:

  // Constructor
  CommandAction();

  // Use reference counting.
  virtual ~CommandAction();

private:

  // No copying.
  CommandAction ( const CommandAction & );
  CommandAction &operator = ( const CommandAction & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_COMMAND_ACTION_CLASS_H_
