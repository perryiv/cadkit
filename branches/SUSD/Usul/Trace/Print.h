
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DEBUG_TRACE_H_
#define _USUL_DEBUG_TRACE_H_

#include "Usul/Export/Export.h"


namespace Usul {
namespace Trace {


class USUL_EXPORT Print
{
public:

  static void close();

  static Print &instance();

protected:

  ~Print();

private:

  Print();

  static Print *_instance;
};


}
}


#endif // _USUL_DEBUG_TRACE_H_
