
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ICREATE_GEOMETRY_H__
#define __USUL_INTERFACES_ICREATE_GEOMETRY_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICreateGeometry : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICreateGeometry );

  /// Id for this interface.
  enum { IID = 3625039624u };

  Usul::Interfaces::IUnknown* createFromBinary ( unsigned int srid, unsigned char* buffer, unsigned int length );

}; // struct ICreateGeometry


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ICREATE_GEOMETRY_H__ */

