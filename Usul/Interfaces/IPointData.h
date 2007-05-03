
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IPOINT_DATA_H__
#define __USUL_INTERFACES_IPOINT_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

namespace Usul {
namespace Interfaces {


struct IPointData : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPointData );

  /// Id for this interface.
  enum { IID = 1916416800u };

  /// Return the point.
  virtual Usul::Math::Vec3d        pointData ( ) = 0;

}; // struct IPointData


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IPOINT_DATA_H__ */

