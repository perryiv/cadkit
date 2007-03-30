
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IDISTRIBUTED_VR_H__
#define __USUL_INTERFACES_IDISTRIBUTED_VR_H__

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Vec3f; }

namespace Usul {
namespace Interfaces {


struct IDistributedVR : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDistributedVR );

  /// Id for this interface.
  enum { IID = 2867749813u };


}; // struct IDistributedVR


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IDISTRIBUTED_VR_H__ */

