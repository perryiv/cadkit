
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Run a local event loop for the given amount of time.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_RUN_LOCAL_EVENT_LOOP_ACTION_CLASS_H_
#define _APP_FRAME_WORK_RUN_LOCAL_EVENT_LOOP_ACTION_CLASS_H_

#include "AppFrameWork/Actions/CommandAction.h"

#include "Usul/Types/Types.h"


namespace AFW {
namespace Actions {


class APP_FRAME_WORK_EXPORT RunLocalEventLoop : public CommandAction
{
public:

  // Typedefs.
  typedef CommandAction BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( RunLocalEventLoop );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( RunLocalEventLoop );

  // Constructor
  RunLocalEventLoop ( Usul::Types::Uint64 duration );

  // Perform the action.
  virtual void                        execute ( AFW::Core::Object *object = 0x0 );

protected:

  // Use reference counting.
  virtual ~RunLocalEventLoop();

private:

  // No copying.
  RunLocalEventLoop ( const RunLocalEventLoop & );
  RunLocalEventLoop &operator = ( const RunLocalEventLoop & );

  Usul::Types::Uint64 _duration;
};


} // namespace Actions
} // namespace AFW


#endif //_APP_FRAME_WORK_RUN_LOCAL_EVENT_LOOP_ACTION_CLASS_H_
