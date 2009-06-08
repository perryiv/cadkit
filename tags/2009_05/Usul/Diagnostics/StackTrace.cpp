
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Utility class for getting and printing the call stack.
//  See: http://www.ibm.com/developerworks/linux/library/l-cppexcep.html
//
//  Note: individual objects are not guarded because all the operations that 
//  write to the object happen in the constructor. The functions that do this 
//  writing are guarded by a single mutex that is shared by all instances.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Diagnostics/StackTrace.h"
#ifdef __GNUC__
#include "Usul/Diagnostics/StackTraceLinux.h"
#elif _MSC_VER
#include "Usul/Diagnostics/StackTraceWindows.h"
#elif __APPLE__
#include "Usul/Diagnostics/StackTraceMac.h"
#else
TODO
#endif
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/RecursiveMutex.h"

#include <iostream>
#include <sstream>
#include <stdio.h>

using namespace Usul::Diagnostics;


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::RecursiveMutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;


///////////////////////////////////////////////////////////////////////////////
//
//  Variables for this file.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul { namespace Diagnostics { namespace Helper { Mutex mutex; } } }


/////////////////////////////////////////////////////////////////////////////
//
//  Default construction grabs the current function stack.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::StackTrace() : _c()
{
  // Need to guard this function for all objects.
  Guard guard ( Usul::Diagnostics::Helper::mutex );

  // Use the helper function.
  Usul::Diagnostics::Helper::getStackTrace ( _c );
}


/////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::StackTrace ( const StackTrace &b ) : _c ( b._c )
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
/////////////////////////////////////////////////////////////////////////////

StackTrace::~StackTrace()
{
}


/////////////////////////////////////////////////////////////////////////////
//
//  Return a string.
//
/////////////////////////////////////////////////////////////////////////////

std::string StackTrace::toString() const
{
  std::ostringstream out;
  for ( Container::const_iterator i = _c.begin(); i != _c.end(); ++i )
  {
    out << (*i) << '\n';
  }
  return out.str();
}


/////////////////////////////////////////////////////////////////////////////
//
//  Print the function signatures to stdout and stderr.
//
/////////////////////////////////////////////////////////////////////////////

void StackTrace::print()
{
  // Some implementations have an optimized way to print.
  bool result ( false );

  // Let the platform implementation try to print.
  {
    // Need to guard this function for all objects.
    Guard guard ( Usul::Diagnostics::Helper::mutex );

    // Use the helper function.
    result = Usul::Diagnostics::Helper::printStackTrace();
  }

  // If there is no platform specific way then do this.
  if ( false == result )
  {
    StackTrace st;
    const std::string s ( st.toString() );
    std::cerr << s << std::endl;
    std::cout << s << std::endl;
  }
}
