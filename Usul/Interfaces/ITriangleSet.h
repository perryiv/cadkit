
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
/// Interface that defines a triangle set.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ITRIANGLESET_H__
#define __USUL_INTERFACES_ITRIANGLESET_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITriangleSet : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangleSet );

  /// Id for this interface.
  enum { IID = 1974062500u };

}; // struct ITriangleSet


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ITRIANGLESET_H__ */
