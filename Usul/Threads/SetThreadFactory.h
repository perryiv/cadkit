
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct. Make a global instance of this if setting in main() is 
//  too late in the program execution.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_THREADS_SET_THREAD_FACTORY_CLASS_H_
#define _USUL_THREADS_SET_THREAD_FACTORY_CLASS_H_

#include "Usul/Threads/Manager.h"

namespace Usul {
namespace Threads {

struct SetThreadFactory
{
  template < class FactoryFunction > SetThreadFactory ( FactoryFunction f, bool replace = false )
  {
    Usul::Threads::Manager::FactoryFunction *current ( Usul::Threads::Manager::instance().factory() );
    if ( 0x0 == current || true == replace )
      Usul::Threads::Manager::instance().factory ( f );
  }
};


} // namespace Threads
} // namespace Usul


#endif // _USUL_THREADS_SET_THREAD_FACTORY_CLASS_H_
