
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

#ifndef _USUL_FUNCTOR_ANALOG_INPUT_BASE_FUNCTOR_H_
#define _USUL_FUNCTOR_ANALOG_INPUT_BASE_FUNCTOR_H_

#include "Usul/Functors/Interaction/Common/BaseFunctor.h"
#include "Usul/Math/Vector2.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Input {


class USUL_EXPORT AnalogInput : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Vec2d Vec2;

  // Type information.
  USUL_DECLARE_TYPE_ID ( AnalogInput );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( AnalogInput );

  // Update the analog input.
  virtual void          operator()() = 0;

  // Set/get the range.
  Vec2                  range() const;
  void                  range ( const Vec2 & );

  // Get the analog input.
  double                value() const { return _value; }

protected:

  explicit AnalogInput ( Unknown *caller, const std::string &name );
  AnalogInput ( const AnalogInput &a );
  virtual ~AnalogInput();

  double _value;
  Vec2 _range;
};


} // namespace Input
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_ANALOG_INPUT_BASE_FUNCTOR_H_
