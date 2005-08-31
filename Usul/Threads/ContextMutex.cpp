
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

#include "Usul/Threads/ContextMutex.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Errors/Stack.h"

#include <stdexcept>
#include <sstream>

using namespace Usul::Threads;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor queries for IOpenGLContext.
//
///////////////////////////////////////////////////////////////////////////////

ContextMutex::ContextMutex ( Usul::Interfaces::IUnknown *unknown ) : BaseClass(),
  _mutex   ( Usul::Threads::Mutex::create() ),
  _context ( unknown )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ContextMutex::~ContextMutex()
{
  // Safely...
  try
  {
    delete _mutex; 
    _mutex = 0x0;
    _context = static_cast < IOpenGLContext * > ( 0x0 );
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    Usul::Errors::Stack::instance().push ( e.what() );
    std::ostringstream out;
    out << "Error 3341511662: Standard exception caught in destructor for OpenGL context mutex: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }

  // Catch all exceptions.
  catch ( ... )
  {
    std::ostringstream out;
    out << "Error 3071151565: Unknown exception caught in destructor for OpenGL context mutex: " << this;
    Usul::Errors::Stack::instance().push ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Lock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void ContextMutex::lock()
{
  _mutex->lock();
  _context->makeContextCurrent();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Unlock the mutex.
//
///////////////////////////////////////////////////////////////////////////////

void ContextMutex::unlock()
{
  _mutex->unlock();
}
