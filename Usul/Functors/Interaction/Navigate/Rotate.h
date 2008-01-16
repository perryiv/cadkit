
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Rotate functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_ROTATION_FUNCTORS_H_
#define _USUL_FUNCTOR_ROTATION_FUNCTORS_H_

#include "Usul/Functors/Interaction/Navigate/Transform.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Navigate {


class USUL_EXPORT Rotate : public Usul::Functors::Interaction::Navigate::Transform
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Navigate::Transform BaseClass;
  typedef BaseClass::Unknown Unknown;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Rotate );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Rotate );

  Rotate ( 
    Unknown *caller = 0x0, 
    const std::string &name = "",
    Direction *dir = 0x0, 
    AnalogInput *ai = 0x0, 
    double speed = 0.1 ); // degrees / second
  Rotate ( const Rotate &cb );

  // Rotate.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

  // Set the maximum degrees/second.
  void                  maxAngularSpeed ( double s );
  double                maxAngularSpeed() const { return this->speed(); }

protected:

  virtual ~Rotate();
};


} // namespace Navigate
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_ROTATION_FUNCTORS_H_
