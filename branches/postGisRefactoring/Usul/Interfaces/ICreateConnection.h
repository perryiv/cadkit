
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICREATE_CONNECTION_H__
#define __USUL_INTERFACES_ICREATE_CONNECTION_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICreateConnection : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICreateConnection );

  /// Id for this interface.
  enum { IID = 2108268141u };

  virtual Usul::Interfaces::IUnknown*       createConnection() = 0;

}; // struct ICreateConnection


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICREATE_CONNECTION_H__ */

