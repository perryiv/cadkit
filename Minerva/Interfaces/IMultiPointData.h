
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IMULTI_POINT_DATA_H__
#define __MINERVA_INTERFACES_IMULTI_POINT_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Minerva {
namespace Interfaces {


struct IMultiPointData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMultiPointData );

  /// Id for this interface.
  enum { IID = 1093077418u };

}; // struct IMultiPointData


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IMULTI_POINT_DATA_H__ */

