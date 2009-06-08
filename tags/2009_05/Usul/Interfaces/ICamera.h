
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for setting the camera
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CAMERA_H_
#define _USUL_INTERFACE_CAMERA_H_

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {


struct ICamera : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ICamera );

  /// Id for this interface.
  enum { IID = 1432227382u };

  // Camera placement flags.
  enum CameraOption { FIT, RESET, FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM, 
                      ROTATE_Y_N45, ROTATE_Y_P45, ROTATE_X_N45, ROTATE_X_P45 };

  // Set the camera.
  virtual void          camera ( CameraOption option ) = 0;

}; // class ICamera


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_CAMERA_H_
