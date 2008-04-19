
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Wrapper for random number generators.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ADAPTORS_RANDOM_H__
#define __USUL_ADAPTORS_RANDOM_H__

#include "Usul/Errors/Assert.h"

#include <cstdlib>


namespace Usul {
namespace Adaptors {


template < class Real > struct Random
{
  Random ( Real minimum, Real maximum ) : 
    _min ( minimum ), 
    _normalize ( ( maximum - minimum ) / static_cast < Real > ( RAND_MAX ) )
  {
    USUL_ASSERT ( minimum < maximum );
  }

  Real operator()() const
  {
    Real number ( static_cast < Real > ( ::rand() ) );
    return _min + _normalize * number;
  }

private:

  Real _min;
  Real _normalize;
};


} // namespace Adaptors
} // namespace Usul


#endif // __USUL_ADAPTORS_RANDOM_H__
