
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ILINE_DATA_H__
#define __USUL_INTERFACES_ILINE_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Geometry; }

namespace Usul {
namespace Interfaces {


struct ILineData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ILineData );

  /// Id for this interface.
  enum { IID = 2143101424u };

  virtual osg::Geometry* buildLineData () = 0;

}; // struct ILineData


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ILINE_DATA_H__ */

