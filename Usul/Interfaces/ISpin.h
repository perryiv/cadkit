
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for toggling spinning
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_SPIN_H_
#define _USUL_INTERFACE_SPIN_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ISpin : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ISpin );

  /// Id for this interface.
  enum { IID = 1758917438u };

  virtual void               spin ( bool ) = 0;

}; // class ISpin


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_SPIN_H_
