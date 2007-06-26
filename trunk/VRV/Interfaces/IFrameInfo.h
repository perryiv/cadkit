
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting information about the frames.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_INTERFACE_FRAME_INFORMATION_H_
#define _VRV_INTERFACE_FRAME_INFORMATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace VRV {
namespace Interfaces {


struct IFrameInfo : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFrameInfo );

  /// Id for this interface.
  enum { IID = 1076366507u };

  /// Get the duration of the last frame in seconds.
  virtual double               frameTime() const = 0;
};


} // namespace Interfaces
} // namespace VRV


#endif // _VRV_INTERFACE_FRAME_INFORMATION_H_
