
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Keep all primitives connected to seed.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_KEEP_ALL_CONNECTED_H_
#define _USUL_INTERFACE_KEEP_ALL_CONNECTED_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IKeepAllConnected : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IKeepAllConnected );

  /// Id for this interface.
  enum { IID = 3312125481u };

  virtual void keepAllConnected ( Usul::Interfaces::IUnknown* caller, unsigned int seed ) = 0;

}; // struct IKeepAllConnected


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_KEEP_LOOPS_H_
