
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

#ifndef _CV_INTERFACE_CLIPPING_DISTANCES_H_
#define _CV_INTERFACE_CLIPPING_DISTANCES_H_

#include "Usul/Interfaces/IUnknown.h"


namespace CV {
namespace Interfaces {


template
<
  unsigned long iid,
  class RealType
>
struct IClippingDistance : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IClippingDistance );

  /// Id for this interface.
  enum { IID = iid };

  /// Get/set the clipping distances.
  virtual void            getClippingDistances ( float &nearDist, float &farDist ) const = 0;
  virtual void            setClippingDistances ( float nearDist, float farDist ) = 0;
};


typedef IClippingDistance<3187721726u,float>  IClippingDistanceFloat;
typedef IClippingDistance<1961562116u,double> IClippingDistanceDouble;


}; // namespace Interfaces
}; // namespace CV


#endif // _CV_INTERFACE_CLIPPING_DISTANCES_H_
