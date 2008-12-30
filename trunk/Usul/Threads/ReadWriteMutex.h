
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Non-recursive multiple-reader, single-writer mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_NON_RECURSIVE_READER_WRITER_MUTEX_CLASS_H_
#define _USUL_THREADS_NON_RECURSIVE_READER_WRITER_MUTEX_CLASS_H_

#include "Usul/Export/Export.h"

namespace Usul { namespace Threads { namespace Detail { struct ReadWriteMutexImpl; } } }


namespace Usul {
namespace Threads {


struct USUL_EXPORT ReadWriteMutex
{
  ReadWriteMutex ( unsigned long writeLockWaitSleep = 100 );
  ~ReadWriteMutex();

  // Lock/unlock for reading.
  void            readLock();
  void            readUnlock();

  // Lock/unlock for writing.
  void            writeLock();
  void            writeUnlock();

private:

  ReadWriteMutex ( const ReadWriteMutex & );             // No copying
  ReadWriteMutex &operator = ( const ReadWriteMutex & ); // No assignment

  Usul::Threads::Detail::ReadWriteMutexImpl *_mutex;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_NON_RECURSIVE_READER_WRITER_MUTEX_CLASS_H_
