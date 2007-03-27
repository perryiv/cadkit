
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

#ifndef _CV_INTERFACE_FRAME_INFORMATION_H_
#define _CV_INTERFACE_FRAME_INFORMATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace CV {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IFrameInfo : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFrameInfo );

  /// Id for this interface.
  enum { IID = iid };

  // Get the duration of the last frame in seconds.
  virtual RealType              frameTime() const = 0;
};


typedef IFrameInfo<1076366507,float>  IFrameInfoFloat;
typedef IFrameInfo<1076366508,double> IFrameInfoDouble;


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_FRAME_INFORMATION_H_
