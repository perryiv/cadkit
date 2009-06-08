
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the frame stamp.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACES_FRAME_STAMP_H_
#define _USUL_INTERFACES_FRAME_STAMP_H_

#include "Usul/Interfaces/IUnknown.h"

namespace osg { class FrameStamp; };


namespace Usul {
namespace Interfaces {


struct IFrameStamp : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFrameStamp );

  /// Id for this interface.
  enum { IID = 2250147716u };

  // Get the frame stamp.
  virtual osg::FrameStamp *         frameStamp() = 0;
  virtual const osg::FrameStamp *   frameStamp() const = 0;
}; 


}
}


#endif // __USUL_INTERFACES_TIMEOUT_ANIMATE_H__

