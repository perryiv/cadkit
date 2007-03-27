
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Mutex classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_MUTEX_CLASSES_H_
#define _USUL_THREADS_MUTEX_CLASSES_H_

#include "Usul/Export/Export.h"


namespace Usul {
namespace Threads {


///////////////////////////////////////////////////////////////////////////////
//
//  Base mutex class.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT Mutex
{
public:

  virtual ~Mutex();

  // The type of function used to create a mutex.
  typedef Mutex *CreateFunction();

  // Create a mutex. Uses the registered create-function.
  static Mutex *          create();

  // Set the create-function. Return the previous one.
  // Note: this functions is not thread-safe!
  static CreateFunction * createFunction ( CreateFunction *fun );
  static CreateFunction * createFunction();

  // Lock/unlock the mutex.
  virtual void            lock() = 0;
  virtual void            unlock() = 0;

protected:

  Mutex();

private:

  Mutex ( const Mutex & );             // No copying
  Mutex &operator = ( const Mutex & ); // No assignment

  static CreateFunction *_fun;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Single-threaded mutex.
//
///////////////////////////////////////////////////////////////////////////////

class USUL_EXPORT SingleThreadedMutex : public Mutex
{
public:

  SingleThreadedMutex();
  virtual ~SingleThreadedMutex();

  virtual void            lock();
  virtual void            unlock();

private:

  SingleThreadedMutex ( const SingleThreadedMutex & );             // No copying
  SingleThreadedMutex &operator = ( const SingleThreadedMutex & ); // No assignment
};


///////////////////////////////////////////////////////////////////////////////
//
//  Creation function.
//
///////////////////////////////////////////////////////////////////////////////

USUL_EXPORT Mutex *newSingleThreadedMutexStub();


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct. Make a global instance of this if setting in main() is 
//  too late in the program execution.
//
///////////////////////////////////////////////////////////////////////////////

struct SetMutexFactory
{
  template < class FactoryFunction > SetMutexFactory ( FactoryFunction f, bool replace = false )
  {
    Usul::Threads::Mutex::CreateFunction *current ( Usul::Threads::Mutex::createFunction() );
    if ( 0x0 == current || true == replace )
      Usul::Threads::Mutex::createFunction ( f );
  }
};


}; // namespace Threads
}; // namespace Usul


#endif // _USUL_THREADS_MUTEX_CLASSES_H_
