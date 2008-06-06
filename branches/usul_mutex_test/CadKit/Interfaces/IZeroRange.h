
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  IZeroRange: Interface for setting the zero range.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_ZERO_RANGE_H_
#define _CADKIT_INTERFACE_ZERO_RANGE_H_

#include "IUnknown.h"


namespace CadKit
{
template <const unsigned int id, class Real> class IZeroRange : public IUnknown
{
public:

  // Id for this interface.
  enum { IID = id };

  // Get the range that gets truncated to zero.
  virtual bool            getZeroRange ( Real &negative, Real &positive ) const = 0;

  // Set the range that gets truncated to zero.
  virtual void            setZeroRange ( const Real &negative, const Real &positive ) = 0;
};


// Common types.
typedef IZeroRange<1043559413,float>       IZeroRangeFloat;
typedef IZeroRange<1043559414,double>      IZeroRangeDouble;
typedef IZeroRange<1043559415,long double> IZeroRangeLongDouble;


}; // namespace CadKit


#endif // _CADKIT_INTERFACE_ZERO_RANGE_H_
