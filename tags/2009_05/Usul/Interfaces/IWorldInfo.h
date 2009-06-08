
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting information about the "world".
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_WORLD_INFORMATION_H_
#define _USUL_INTERFACE_WORLD_INFORMATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IWorldInfo : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWorldInfo );

  /// Id for this interface.
  enum { IID = 1076369080u };

  // Get the radius of the "world".
  virtual double              worldRadius() const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_WORLD_INFORMATION_H_
