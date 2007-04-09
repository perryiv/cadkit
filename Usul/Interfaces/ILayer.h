
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILAYER_H__
#define __USUL_INTERFACES_ILAYER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ILayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILayer );

  /// Id for this interface.
  enum { IID = 1743049682u };

}; // struct ILayer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ILAYER_H__ */

