
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wand position matrix functor.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_WAND_POSITION_MATRIX_FUNCTORS_H_
#define _CV_WAND_POSITION_MATRIX_FUNCTORS_H_

#include "CadViewer/Functors/WandMatrix.h"


namespace CV {
namespace Functors {


class WandPosition : public WandMatrix
{
public:

  // Useful typedef(s).
  USUL_DECLARE_REF_POINTERS ( WandPosition );
  typedef WandMatrix BaseClass;
  typedef BaseClass::Unknown Unknown;
  typedef Usul::Math::Matrix44f Matrix;

  explicit WandPosition ( Unknown *unknown );
  WandPosition ( const WandPosition &a );

  // Update the matrix.
  virtual void          operator()();

protected:

  virtual ~WandPosition();
};


}; // namespace Functors
}; // namespace CV


#endif // _CV_WAND_POSITION_MATRIX_FUNCTORS_H_
