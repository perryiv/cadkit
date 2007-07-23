
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand rotation matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_WAND_ROTATION_MATRIX_FUNCTORS_H_
#define _VRV_WAND_ROTATION_MATRIX_FUNCTORS_H_

#include "VRV/Export.h"
#include "VRV/Functors/Wand/WandMatrix.h"


namespace VRV {
namespace Functors {


class VRV_EXPORT WandRotation : public WandMatrix
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( WandRotation );
  typedef WandMatrix BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  explicit WandRotation ( Unknown *unknown );
  WandRotation ( const WandRotation &a );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~WandRotation();
};


}; // namespace Functors
}; // namespace VRV


#endif // _VRV_WAND_ROTATION_MATRIX_FUNCTORS_H_
