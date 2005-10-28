
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command action for closing the active editor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_CLOSE_ACTIVE_EDITOR_ACTION_CLASS_H_
#define _APP_FRAME_WORK_CLOSE_ACTIVE_EDITOR_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"

#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT CloseActiveEditor : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( CloseActiveEditor );

  // Constructor
  CloseActiveEditor();

  // Perform the action.
  virtual void                        execute();

protected:

  // Use reference counting.
  virtual ~CloseActiveEditor();

private:

  // No copying.
  CloseActiveEditor ( const CloseActiveEditor & );
  CloseActiveEditor &operator = ( const CloseActiveEditor & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_CLOSE_ACTIVE_EDITOR_ACTION_CLASS_H_
