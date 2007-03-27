
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for exiting the program.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_QUIT_ACTION_CLASS_H_
#define _APP_FRAME_WORK_QUIT_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT QuitProgramAction : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( QuitProgramAction );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( QuitProgramAction );

  // Constructor
  QuitProgramAction();

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~QuitProgramAction();

private:

  // No copying.
  QuitProgramAction ( const QuitProgramAction & );
  QuitProgramAction &operator = ( const QuitProgramAction & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_QUIT_ACTION_CLASS_H_
