
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Thread class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_THREADS_EXTENSIONS_THREAD_H_
#define _OPEN_THREADS_EXTENSIONS_THREAD_H_

#include "Threads/OpenThreads/Export.h"

#include "Usul/Threads/Thread.h"

namespace OpenThreads { class Thread; }
namespace Threads { namespace OT { namespace Detail { class OpenThread; } } }


namespace Threads {
namespace OT {


class OPEN_THREADS_EXTENSIONS_EXPORT Thread : public Usul::Threads::Thread
{
public:

  typedef Usul::Threads::Thread BaseClass;

  Thread ( const std::string &name );

  virtual bool            isIdle() const;

protected:

  virtual ~Thread();

  virtual void            _start();

private:

  Thread ( const Thread & );             // No copying
  Thread &operator = ( const Thread & ); // No assignment

  void                    _destroy();

  friend class Detail::OpenThread;

  OpenThreads::Thread *_thread;
};


// Creation function.
OPEN_THREADS_EXTENSIONS_EXPORT Usul::Threads::Thread::RefPtr newOpenThreadsThread ( const std::string &name );


}; // namespace OT
}; // namespace Threads


#endif // _OPEN_THREADS_EXTENSIONS_THREAD_H_
