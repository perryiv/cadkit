
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_WAND_MATRIX_FUNCTOR_H_
#define _CV_WAND_MATRIX_FUNCTOR_H_

#include "VRV/Functors/Matrix/MatrixFunctor.h"
#include "VRV/Interfaces/IWandState.h"


namespace CV {
namespace Functors {


class WandMatrix : public VRV::Functors::Matrix::MatrixFunctor
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( WandMatrix );
  typedef VRV::Functors::Matrix::MatrixFunctor BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef VRV::Interfaces::IWandStateFloat WandState;
  typedef Usul::Math::Matrix44f Matrix;

  explicit WandMatrix ( Unknown *unknown );
  WandMatrix ( const WandMatrix &a );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~WandMatrix();

  const WandState *   _wandState() const { return _ws.get(); }
  WandState *         _wandState()       { return _ws.get(); }

private:

  WandState::ValidQueryPtr _ws;
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_WAND_MATRIX_FUNCTOR_H_
