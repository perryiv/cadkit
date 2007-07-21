
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

#ifndef _VRV_DIRECTION_VECTOR_FUNCTOR_H_
#define _VRV_DIRECTION_VECTOR_FUNCTOR_H_

#include "VRV/Export.h"
#include "VRV/Functors/Matrix/MatrixFunctor.h"

#include "Usul/Math/Vector3.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT Direction : public VRV::Functors::BaseFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( Direction );
  typedef VRV::Functors::BaseFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Vec3f Vector;
  typedef VRV::Functors::Matrix::MatrixFunctor MatrixFunctor;

  Direction ( Unknown *unknown, const Vector &dir, MatrixFunctor *mf );
  Direction ( const Direction &d );

  // Update the direction.
  virtual void          operator()();

  // Get the direction.
  const Vector &        vector() const { return _dir; }

protected:

  virtual ~Direction();

private:

  Vector _dir;
  Vector _original;
  MatrixFunctor::RefPtr _mf;
};


} // namespace Functors
} // namespace VRV


#endif // _VRV_DIRECTION_VECTOR_FUNCTOR_H_
