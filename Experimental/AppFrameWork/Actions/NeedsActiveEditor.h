
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Update according to the condition.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_NEEDS_ACTIVE_EDITOR_UPDATE_ACTION_CLASS_H_
#define _APP_FRAME_WORK_NEEDS_ACTIVE_EDITOR_UPDATE_ACTION_CLASS_H_

#include "AppFrameWork/Actions/UpdateAction.h"

#include "Usul/Pointers/Pointers.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT NeedsActiveEditor : public UpdateAction
{
public:

  // Typedefs.
  typedef UpdateAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( NeedsActiveEditor );

  // Constructor
  NeedsActiveEditor();

  // Get the proper state.
  virtual bool                        state();

protected:

  // Use reference counting.
  virtual ~NeedsActiveEditor();

private:

  // No copying.
  NeedsActiveEditor ( const NeedsActiveEditor & );
  NeedsActiveEditor &operator = ( const NeedsActiveEditor & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_NEEDS_ACTIVE_EDITOR_UPDATE_ACTION_CLASS_H_
