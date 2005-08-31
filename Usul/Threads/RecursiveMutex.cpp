
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Recursive mutex class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Errors/Stack.h"

#include <stdexcept>
#include <sstream>

using namespace Usul;
using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Useful typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Guard < Usul::Threads::Mutex > MutexGuard;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

RecursiveMutex::RecursiveMutex() :
  _mutex ( Usul::Threads::Mutex::create() ),
  _local ( Usul::Threads::Mutex::create() ),
  _count ( 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

RecursiveMutex::~RecursiveMutex()
{
  USUL_ASSERT ( 0 == _count );

  // Safely...
  try
  {
    delete _local;
    _local = 0x0;
    delete _mutex;
    _mutex = 0x0;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream out;
    out << "Error 2795795243: Standard exception caught in destructor for recursive mutex: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 2316729842: Unknown exception caught in destructor for recursive mutex: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void RecursiveMutex::lock()
{
  // One thread at a time in this function.
  MutexGuard guard ( *_local );

  // Lock mutex if we should.
  if ( 0 == _count )
    _mutex->lock();

  // Increment the count.
  ++_count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void RecursiveMutex::unlock()
{
  // One thread at a time in this function.
  MutexGuard guard ( *_local );

  // Decrement the count.
  --_count;

  // Unlock mutex if we should.
  if ( 0 == _count )
    _mutex->unlock();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of times this mutex has been locked.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int RecursiveMutex::count() const
{
  // One thread at a time in this function.
  MutexGuard guard ( *_local );

  // This should be a little safer than "return _count".
  const unsigned int c ( _count );
  return c;
}
