
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Return an about string.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IABOUTSTRING_H__
#define __USUL_INTERFACES_IABOUTSTRING_H__

#include "Usul/Interfaces/IUnknown.h"

#include <string>

namespace Usul {
namespace Interfaces {


struct IAboutString : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAboutString );

  /// Id for this interface.
  enum { IID = 1100312u };

  virtual std::string       aboutString() const = 0;
}; // struct IAboutString


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IABOUTSTRING_H__ */
