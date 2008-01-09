
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ITIMESTEP_ANIMATION_DATA_H__
#define __USUL_INTERFACES_ITIMESTEP_ANIMATION_DATA_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ITimestepAnimation : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITimestepAnimation );

  /// Id for this interface.
  enum { IID = 4127260732u };

  virtual void                startTimestepAnimation () = 0;
  virtual void                stopTimestepAnimation () = 0;

}; // struct ITimestepAnimation


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_ITIMESTEP_ANIMATION_DATA_H__ */

