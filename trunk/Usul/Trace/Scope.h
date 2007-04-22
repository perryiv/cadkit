
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
//  Class for Scopeing when entering and leaving a scope.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DEBUG_TRACE_SCOPE_H_
#define _USUL_DEBUG_TRACE_SCOPE_H_

#include "Usul/Export/Export.h"

#include <string>


namespace Usul {
namespace Trace {


class USUL_EXPORT Scope
{
public:

  Scope ( const std::string &name );
  ~Scope();

private:

  std::string _name;
};


}
}


#endif // _USUL_DEBUG_TRACE_SCOPE_H_
