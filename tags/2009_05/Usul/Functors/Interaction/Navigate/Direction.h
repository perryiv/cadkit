
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor for getting a direction.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_FUNCTOR_DIRECTION_VECTOR_FUNCTOR_H_
#define _USUL_FUNCTOR_DIRECTION_VECTOR_FUNCTOR_H_

#include "Usul/Functors/Interaction/Matrix/MatrixFunctor.h"
#include "Usul/Math/Vector3.h"


namespace Usul {
namespace Functors {
namespace Interaction {
namespace Navigate {


class USUL_EXPORT Direction : public Usul::Functors::Interaction::Common::BaseFunctor
{
public:

  // Useful typedef(s).
  typedef Usul::Functors::Interaction::Common::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Vec3d Vector;
  typedef Usul::Functors::Interaction::Matrix::MatrixFunctor MatrixFunctor;

  // Type information.
  USUL_DECLARE_TYPE_ID ( Direction );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Direction );

  Direction ( Unknown *caller = 0x0, 
              const std::string &name = "", 
              const Vector &dir = Vector ( 0.0f, 0.0f, -1.0f ), 
              MatrixFunctor *mf = 0x0 );
  Direction ( const Direction &d );

  // Set matrix functor.
  void                  matrix ( MatrixFunctor * );

  // Set/get the original direction.
  Vector                original() const;
  void                  original ( const Vector & );

  // Update the direction.
  virtual void          operator()();

  // Clone this functor.
  virtual BaseFunctor*  clone ();

  // Return the current direction.
  Vector                vector() const;

protected:

  virtual ~Direction();

private:

  Vector _dir;
  Vector _original;
  MatrixFunctor::RefPtr _mf;
};


} // namespace Navigate
} // namespace Interaction
} // namespace Functors
} // namespace Usul


#endif // _USUL_FUNCTOR_DIRECTION_VECTOR_FUNCTOR_H_
