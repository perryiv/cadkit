
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
//  Default value of 10 for unitsInLastPlace is from Dawson's code [3].
//
//  [1] http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
//  [2] http://www.lomont.org/Math/Papers/2005/CompareFloat.pdf
//  [3] ftp://ftp.cygnus-software.com/pub/comparecode.zip
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_PREDICATES_CLOSE_FLOAT_H_
#define _USUL_PREDICATES_CLOSE_FLOAT_H_

#include "Usul/Types/Types.h"
#include "Usul/Math/Finite.h"
#include "Usul/Math/NaN.h"
#include "Usul/Math/Absolute.h"


namespace Usul {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > struct CloseFloat;


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for Usul::Types::Float32.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct CloseFloat < Usul::Types::Float32 >
{
  // Useful typedefs.
  typedef Usul::Types::Float32 FloatType;
  typedef Usul::Types::Int32 SignedInteger;
  typedef Usul::Types::Uint32 UnsignedInteger;
  typedef CloseFloat < FloatType > ThisType;

  // Make this predicate adaptable.
	typedef FloatType first_argument_type;
	typedef FloatType second_argument_type;
	typedef bool result_type;

  // Let the compiler make copy constructor and assignment operator.
  explicit CloseFloat ( unsigned int unitsInLastPlace = 10 ) : _ulps ( unitsInLastPlace ){}

  static bool compare ( FloatType a, FloatType b, UnsignedInteger unitsInLastPlace )
  {
    // If either are infinity, then they are "close" iff they are exact.
    if ( ( false == Usul::Math::finite ( a ) ) || ( false == Usul::Math::finite ( b ) ) )
      return a == b;

    // If either are NAN, then they cannot be equal, even if they are identical.
    if ( Usul::Math::nan ( a ) || Usul::Math::nan ( b ) )
      return false;

    // Interpret the memory as an int.
    SignedInteger ia = *(reinterpret_cast<SignedInteger*>(&a));
    SignedInteger ib = *(reinterpret_cast<SignedInteger*>(&b));

    // Make them lexicographically ordered as a twos-complement int.
    if ( ia < 0 )
      ia = 0x80000000 - ia;
    if ( ib < 0 )
      ib = 0x80000000 - ib;

    // See how far apart a and b are.
    UnsignedInteger diff ( static_cast < UnsignedInteger > ( Usul::Math::absolute ( ia - ib ) ) );
    return ( diff <= unitsInLastPlace );
  }

  bool operator () ( FloatType a, FloatType b ) const
  {
    return ThisType::compare ( a, b, _ulps );
  }

protected:

  UnsignedInteger _ulps;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for Usul::Types::Float64.
//
///////////////////////////////////////////////////////////////////////////////

template <> struct CloseFloat < Usul::Types::Float64 >
{
  // Useful typedefs.
  typedef Usul::Types::Float64 FloatType;
  typedef Usul::Types::Int64 SignedInteger;
  typedef Usul::Types::Uint64 UnsignedInteger;
  typedef CloseFloat < FloatType > ThisType;

  // Make this predicate adaptable.
	typedef FloatType first_argument_type;
	typedef FloatType second_argument_type;
	typedef bool result_type;

  // Let the compiler make copy constructor and assignment operator.
  explicit CloseFloat ( unsigned int unitsInLastPlace = 10 ) : _ulps ( unitsInLastPlace ){}

  static bool compare ( FloatType a, FloatType b, UnsignedInteger unitsInLastPlace )
  {
    // If either are infinity, then they are "close" iff they are exact.
    if ( ( false == Usul::Math::finite ( a ) ) || ( false == Usul::Math::finite ( b ) ) )
      return a == b;

    // If either are NAN, then they cannot be equal, even if they are identical.
    if ( Usul::Math::nan ( a ) || Usul::Math::nan ( b ) )
      return false;

    // Interpret the memory as an int.
    SignedInteger ia = *(reinterpret_cast<SignedInteger*>(&a));
    SignedInteger ib = *(reinterpret_cast<SignedInteger*>(&b));

    // Make them lexicographically ordered as a twos-complement int.
    if ( ia < 0 )
      ia = 0x8000000000000000ull - ia;
    if ( ib < 0 )
      ib = 0x8000000000000000ull - ib;

    // See how far apart a and b are.
    UnsignedInteger diff ( static_cast < UnsignedInteger > ( Usul::Math::absolute ( ia - ib ) ) );
    return ( diff <= unitsInLastPlace );
  }

  bool operator () ( FloatType a, FloatType b ) const
  {
    return ThisType::compare ( a, b, _ulps );
  }

protected:

  UnsignedInteger _ulps;
};


}; // namespace Predicates
}; // namespace Usul


#endif // _USUL_PREDICATES_CLOSE_FLOAT_H_
