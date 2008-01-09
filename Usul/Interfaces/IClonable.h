
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICLONABLE_H__
#define __USUL_INTERFACES_ICLONABLE_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IClonable : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClonable );

  /// Id for this interface.
  enum { IID = 3517720390u };

  virtual Usul::Interfaces::IUnknown*    clone() const = 0;

}; // struct IClonable


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICLONABLE_H__ */

