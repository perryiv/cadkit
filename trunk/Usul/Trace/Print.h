
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
//  Class for printing a string.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DEBUG_TRACE_PRINT_H_
#define _USUL_DEBUG_TRACE_PRINT_H_

#include "Usul/Export/Export.h"

#include <string>
#include <iostream>


namespace Usul {
namespace Trace {


struct USUL_EXPORT Print
{
  static void execute ( const std::string & );
  static void execute ( const char * );
  static void init ( std::ostream * );
};


}
}


#endif // _USUL_DEBUG_TRACE_PRINT_H_
