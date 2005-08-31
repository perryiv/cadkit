
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mutex that guards the OpenGL context.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_OPENGL_CONTEXT_MUTEX_H_
#define _USUL_THREADS_OPENGL_CONTEXT_MUTEX_H_

#include "Usul/Export/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IOpenGLContext.h"


namespace Usul {
namespace Threads {

class Mutex;

class USUL_EXPORT ContextMutex : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IOpenGLContext IOpenGLContext;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ContextMutex );

  // Constructor queries for IOpenGLContext.
  ContextMutex ( Usul::Interfaces::IUnknown *caller = 0x0 );

  // Lock the mutex and make context current.
  void              lock();

  // Unlock the mutex.
  void              unlock();

protected:

  // Use reference counting.
  ~ContextMutex();

private:

  ContextMutex ( const ContextMutex & );             // No copying
  ContextMutex &operator = ( const ContextMutex & ); // No assignment

  Usul::Threads::Mutex *_mutex;
  IOpenGLContext::ValidAccessQueryPtr _context;
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_OPENGL_CONTEXT_MUTEX_H_
