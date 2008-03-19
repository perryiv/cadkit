
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ITEMPORAL_DATA_H__
#define __MINERVA_INTERFACES_ITEMPORAL_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct ITemporalData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITemporalData );

  /// Id for this interface.
  enum { IID = 2307021671u };

}; // struct ITemporalData


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_ITEMPORAL_DATA_H__ */

