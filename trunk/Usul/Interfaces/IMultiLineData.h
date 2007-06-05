
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMULTI_LINE_DATA_H__
#define __USUL_INTERFACES_IMULTI_LINE_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct IMultiLineData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMultiLineData );

  /// Id for this interface.
  enum { IID = 3602473287u };

}; // struct IMultiLineData


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IMULTI_LINE_DATA_H__ */

