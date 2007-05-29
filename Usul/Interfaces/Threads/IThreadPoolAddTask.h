
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Interface for adding a task to the thread pool.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_THREAD_POOL_ADD_TASK_H_
#define _USUL_INTERFACE_THREAD_POOL_ADD_TASK_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul { namespace Threads { class Callback; } }


namespace Usul {
namespace Interfaces {


struct IThreadPoolAddTask : public Usul::Interfaces::IUnknown
{
  typedef unsigned long TaskHandle;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IThreadPoolAddTask );

  /// Id for this interface.
  enum { IID = 4196711123u };

  /// Add a task.
  virtual TaskHandle addTask ( Usul::Threads::Callback *started, 
                               Usul::Threads::Callback *finished  = 0x0,
                               Usul::Threads::Callback *cancelled = 0x0,
                               Usul::Threads::Callback *error     = 0x0,
                               Usul::Threads::Callback *destroyed = 0x0 ) = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_THREAD_POOL_ADD_TASK_H_
