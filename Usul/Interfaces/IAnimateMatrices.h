
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_ANIMATE_MATRICES_H__
#define __USUL_INTERFACES_ANIMATE_MATRICES_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Matrix44.h"

#include <vector>


namespace Usul {
namespace Interfaces {


struct IAnimateMatrices : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef Usul::Math::Matrix44d Matrix;
  typedef std::vector < Matrix > Matrices;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IAnimateMatrices );

  /// Id for this interface.
  enum { IID = 2715663365u };

  // Animate through the path of matrices.
  virtual void      animateMatrices ( const Matrices &, unsigned int milliseconds, bool loop, Usul::Interfaces::IUnknown::RefPtr caller ) = 0;
};


}
}

#endif // __USUL_INTERFACES_ANIMATE_MATRICES_H__
