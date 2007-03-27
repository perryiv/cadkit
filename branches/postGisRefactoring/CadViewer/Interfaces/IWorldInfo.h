
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting information about the "world".
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_INTERFACE_WORLD_INFORMATION_H_
#define _CV_INTERFACE_WORLD_INFORMATION_H_

#include "Usul/Interfaces/IUnknown.h"


namespace CV {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IWorldInfo : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IWorldInfo );

  /// Id for this interface.
  enum { IID = iid };

  // Get the radius of the "world".
  virtual RealType              worldRadius() const = 0;
};


typedef IWorldInfo<1076369079,float>  IWorldInfoFloat;
typedef IWorldInfo<1076369080,double> IWorldInfoDouble;


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_WORLD_INFORMATION_H_
