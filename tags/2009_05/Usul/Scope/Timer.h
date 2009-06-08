
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_SCOPE_TIMER_H__
#define __USUL_SCOPE_TIMER_H__

#include "Usul/Export/Export.h"

#include <string>

namespace Usul {
namespace Scope {

class USUL_EXPORT Timer
{
public:
  Timer ( const std::string& message );

  ~Timer();
private:
  double _start;
  std::string _message;
};

}
}

#endif // __USUL_SCOPE_TIMER_H__
