
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Translation functors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_TRANSLATION_FUNCTORS_H_
#define _USUL_FUNCTOR_TRANSLATION_FUNCTORS_H_

#include "Usul/Functors/Interaction/Navigate/Transform.h"
#include "Usul/Interfaces/ITranslationSpeed.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Navigate {


class USUL_EXPORT Translate : public Usul::Functors::Interaction::Navigate::Transform
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Navigate::Transform BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Interfaces::ITranslationSpeed ITranslationSpeed;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Translate );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Translate );

  Translate (
    Unknown *caller = 0x0, 
    const std::string &name = "", 
    Direction *dir = 0x0, 
    AnalogInput *ai = 0x0,
    double speed = 0.1 ); // percentage-of-world-radius / seconds
  Translate ( const Translate &cb );

  // Translate.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

  // Set/get the maximum relative speed. The units are 
  // percentage-of-world-radius / seconds.
  void                  maxRelativeSpeed ( double s );
  double                maxRelativeSpeed() const { return this->speed(); }

  // Get the current speed.
  double                currentSpeed() const;

  // Set the caller.
  virtual void          caller ( Unknown* caller );

protected:

  virtual ~Translate();

private:

  ITranslationSpeed::QueryPtr _translateSpeed;
};


} // namespace Navigate
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_TRANSLATION_FUNCTORS_H_
