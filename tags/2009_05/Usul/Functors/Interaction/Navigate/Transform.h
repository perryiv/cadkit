
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base callback class for functors that transform the scene.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_TRANFORM_FUNCTORS_H_
#define _USUL_FUNCTOR_TRANFORM_FUNCTORS_H_

#include "Usul/Functors/Interaction/Input/AnalogInput.h"
#include "Usul/Functors/Interaction/Navigate/Direction.h"
#include "Usul/Interfaces/IFrameInfo.h"
#include "Usul/Interfaces/IWorldInfo.h"
#include "Usul/Interfaces/IMatrixMultiply.h"
#include "Usul/Math/Matrix44.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Navigate {


class USUL_EXPORT Transform : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef Usul::Functors::Interaction::Navigate::Direction Direction;
  typedef Usul::Functors::Interaction::Input::AnalogInput AnalogInput;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Transform );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Transform );

  // Set the analog functor.
  void                  analog ( AnalogInput * );

  // Set the direction functor.
  void                  direction ( Direction * );

  // Set/get the speed.
  double                speed() const;
  void                  speed ( double );

  // Set the caller.
  virtual void          caller ( Unknown* caller );

protected:

  typedef BaseClass::Unknown Unknown;
  typedef Direction::Vector Vector;
  typedef Usul::Math::Matrix44d Matrix;
  typedef Usul::Interfaces::IFrameInfo FrameInfo;
  typedef Usul::Interfaces::IWorldInfo WorldInfo;
  typedef Usul::Interfaces::IMatrixMultiplyDouble MatrixMultiply;

  Transform (
    Unknown *caller, 
    const std::string &name,
    Direction *dir,
    AnalogInput *ai,
    double speed );
  Transform ( const Transform &cb );
  virtual ~Transform();

  // Get the analog input.
  double                _analog();

  // Get the current direction.
  Vector                _direction();

  // Get the amount of time the last frame took.
  double                _frameTime() const;

  // Post or pre-multiply the current matrix.
  void                  _postMult ( const Matrix & );
  void                  _preMult  ( const Matrix & );

  // Get the radius of the "world".
  double                _worldRadius() const;

private:

  Direction::RefPtr _dir;
  AnalogInput::RefPtr _ai;
  FrameInfo::QueryPtr _fi;
  WorldInfo::QueryPtr _wi;
  MatrixMultiply::QueryPtr _mm;
  double _speed;
};


} // namespace Navigate
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_TRANFORM_FUNCTORS_H_
