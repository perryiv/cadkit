
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_STATE_BUSY_H__
#define __USUL_STATE_BUSY_H__

#include "Usul/Export/Export.h"

namespace Usul {
namespace State {

struct USUL_EXPORT Busy
{
  Busy();
  ~Busy(); 
};


USUL_EXPORT bool busy();


}
}

#endif //__USUL_STATE_BUSY_H__

