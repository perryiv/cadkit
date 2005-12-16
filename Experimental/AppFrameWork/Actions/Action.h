
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all actions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_ACTION_CLASS_H_
#define _APP_FRAME_WORK_ACTION_CLASS_H_

#include "AppFrameWork/Core/Object.h"


namespace AFW {
namespace Actions {


  class APP_FRAME_WORK_EXPORT Action : public AFW::Core::Object
{
public:

  // Typedefs.
  typedef AFW::Core::Object BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Action );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( Action );

  // Can we undo the action?
  virtual bool                        canUndo ( AFW::Core::Object *object = 0x0 ) const;

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 ) = 0;

  // Undo the command.
  virtual void                        undo ( AFW::Core::Object *object = 0x0 );

protected:

  // Constructor
  Action();

  // Use reference counting.
  virtual ~Action();

private:

  // No copying.
  Action ( const Action & );
  Action &operator = ( const Action & );
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_ACTION_CLASS_H_
