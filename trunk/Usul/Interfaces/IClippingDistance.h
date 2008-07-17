
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting and setting the clipping distances.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_CLIPPING_DISTANCES_H_
#define _USUL_INTERFACE_CLIPPING_DISTANCES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace Usul {
namespace Interfaces {


struct IClippingDistance : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClippingDistance );

  /// Id for this interface.
  enum { IID = 1961562116u };

  /// Get/set the clipping distances.
  virtual void            getClippingDistances ( double &nearDist, double &farDist ) const = 0;
  virtual void            setClippingDistances ( double nearDist, double farDist ) = 0;
};


} // namespace Interfaces
} // namespace VRV


#endif // _USUL_INTERFACE_CLIPPING_DISTANCES_H_
