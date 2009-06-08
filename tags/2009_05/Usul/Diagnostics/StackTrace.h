
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
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DIAGNOSTICS_STACK_TRACE_H_
#define _USUL_DIAGNOSTICS_STACK_TRACE_H_

#include "Usul/Export/Export.h"

#include <string>
#include <vector>


namespace Usul {
namespace Diagnostics {


class USUL_EXPORT StackTrace
{
public:

  typedef std::vector < std::string > Container;

  // Default construction grabs the current function stack.
  StackTrace();
  StackTrace ( const StackTrace & );

  // Destructor.
  ~StackTrace();

  // Get internal container. Nothing writes to _c after the 
  // constructor, so not need to guard.
  Container                 get() const { return _c; }
  template < class T > void get ( T &c ) const { c = _c; }

  // Return a string.
  std::string               toString() const;

  // Print the stack trace to stdout and stderr.
  static void               print();

private:

  Container _c;
};


} // namespace Diagnostics
} // namespace Usul


#endif // _USUL_DIAGNOSTICS_STACK_TRACE_H_
