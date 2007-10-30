
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate classes to test if two floating-point numbers are "close". 
//  See Bruce Dawson's paper [1] for details.
//
//  In most cases, this approach is much better than specifying a tolerance 
//  because, as Chris Lomont asserts in [2], "...the tolerance required 
//  depends on the size of the values being tested."
//
//  Lomont's paper describes a solution that executes faster than Dawson's, 
//  but is the method platform independent?
//
//  Dawson's code [3] had a default value of 10 for unitsInLastPlace.
//
//  [1] http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
//  [2] http://www.lomont.org/Math/Papers/2005/CompareFloat.pdf
//  [3] ftp://ftp.cygnus-software.com/pub/comparecode.zip
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_CLOSE_FLOAT_H_
#define _USUL_PREDICATES_CLOSE_FLOAT_H_

#include "Usul/Errors/Assert.h"
#include "Usul/Cast/Cast.h"
#include "Usul/Math/Absolute.h"
#include "Usul/Math/Finite.h"
#include "Usul/Math/NaN.h"
#include "Usul/Types/Types.h"


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions for close-float functor below.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline void handleTwosCompliment ( Usul::Types::Int32 &v )
  {
    if ( v < 0 )
      v = 0x80000000 - v;
  }
  inline void handleTwosCompliment ( Usul::Types::Int64 &v )
  {
    if ( v < 0 )
      v = 0x8000000000000000ull - v;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Type-traits for close-float functor below.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class FloatType > struct IntegerSelector;
  template <> struct IntegerSelector < Usul::Types::Float32 >
  {
    typedef Usul::Types::Int32 SignedInteger;
    typedef Usul::Types::Uint32 UnsignedInteger;
  };
  template <> struct IntegerSelector < Usul::Types::Float64 >
  {
    typedef Usul::Types::Int64 SignedInteger;
    typedef Usul::Types::Uint64 UnsignedInteger;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns true if given floating-point values are "close".
//
///////////////////////////////////////////////////////////////////////////////

template < class FloatType_ > struct CloseFloat
{
  // Useful typedefs.
  typedef FloatType_ FloatType;
  typedef Usul::Predicates::Detail::IntegerSelector<FloatType> IntegerSelector;
  typedef typename IntegerSelector::SignedInteger SignedInteger;
  typedef typename IntegerSelector::UnsignedInteger UnsignedInteger;
  typedef CloseFloat < FloatType > ThisType;

  // Make this predicate adaptable.
	typedef FloatType first_argument_type;
	typedef FloatType second_argument_type;
	typedef bool result_type;

  // Let the compiler make copy constructor and assignment operator.
  // Note: previous default of 10 was causing std::map::find to return false 
  // but then std::map::insert would return an existing value, when this class
  // was used with LessVector as the map's comparison predicate. Not exactly 
  // sure why but tightening up the specified difference made the problem go away.
  explicit CloseFloat ( unsigned int unitsInLastPlace = 1 ) : _ulps ( unitsInLastPlace ){}

  // Compare the two numbers.
  static bool compare ( FloatType a, FloatType b, UnsignedInteger unitsInLastPlace )
  {
    // If either are infinity, then they are "close" iff they are exact.
    if ( ( false == Usul::Math::finite ( a ) ) || ( false == Usul::Math::finite ( b ) ) )
    {
      USUL_ASSERT ( 0 ); // Heads up. Make this a policy if infinity is valid.
      return a == b;
    }

    // If either are NAN, then they cannot be equal, even if they are identical.
    if ( Usul::Math::nan ( a ) || Usul::Math::nan ( b ) )
    {
      USUL_ASSERT ( 0 ); // Heads up. Make this a policy if NAN is valid.
      return false;
    }

    // Interpret the memory as a signed integer.
    SignedInteger ia ( *( USUL_UNSAFE_CAST ( SignedInteger*, &a ) ) );
    SignedInteger ib ( *( USUL_UNSAFE_CAST ( SignedInteger*, &b ) ) );

    // Make them lexicographically ordered as a twos-complement int.
    Usul::Predicates::Detail::handleTwosCompliment ( ia );
    Usul::Predicates::Detail::handleTwosCompliment ( ib );

    // See how far apart a and b are.
    const UnsignedInteger diff ( static_cast < UnsignedInteger > ( Usul::Math::absolute ( ia - ib ) ) );

#ifdef _DEBUG
    if ( diff <= unitsInLastPlace )
    {
      if ( a != b )
      {
        return true; // "Close" but not equal.
      }
    }
#endif

    // They are "close" if the difference is within the specified amount.
    return ( diff <= unitsInLastPlace );
  }

  bool operator () ( FloatType a, FloatType b ) const
  {
    return ThisType::compare ( a, b, _ulps );
  }

protected:

  UnsignedInteger _ulps;
};


} // namespace Predicates
} // namespace Usul


#endif // _USUL_PREDICATES_CLOSE_FLOAT_H_
