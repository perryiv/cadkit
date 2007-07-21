
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base functor class for analog input.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_ANALOG_INPUT_BASE_FUNCTOR_H_
#define _VRV_ANALOG_INPUT_BASE_FUNCTOR_H_

#include "VRV/Export.h"
#include "VRV/Functors/BaseFunctor.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT AnalogInput : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( AnalogInput );
  typedef VRV::Functors::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;

  // Update the analog input.
  virtual void          operator()() = 0;

  // Get the analog input.
  float                 value() const { return _value; }

protected:

  explicit AnalogInput ( Unknown *unknown );
  AnalogInput ( const AnalogInput &a );
  virtual ~AnalogInput();

  float _value;
};


} // namespace Functors
} // namespace VRV


#endif // _VRV_ANALOG_INPUT_BASE_FUNCTOR_H_
