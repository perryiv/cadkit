
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The finite function.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_MATH_FINITE_FUNCTION_H_
#define _USUL_MATH_FINITE_FUNCTION_H_

#include "Usul/Cast/Cast.h"


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define THROW for the various platforms.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef __sgi
  #define _USUL_THROW
#elif __CYGWIN__
  #define _USUL_THROW
#elif __APPLE__
  #define _USUL_THROW
#else
  #define _USUL_THROW throw()
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Macro define Visual Studio code annotations.
//  These macros ensures that the function signature is the same as in Microsoft provided headers.
//  This is to fix a warning with Visual Studio 2008.
//  See: http://msdn.microsoft.com/en-us/library/cc664879.aspx
//
///////////////////////////////////////////////////////////////////////////////

#if defined ( _MSC_VER ) && _MSC_VER > 1400
#include <sal.h>
#define _USUL_IN _In_
#define _USUL_CHECK_RETURN_ _Check_return_
#else
#define _USUL_IN
#define _USUL_CHECK_RETURN_
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Both cmath and math.h are a pain. 
//
//  Note: Needed to add 'throws()' so that Linux gcc would accept the 
//  prototypes in a source file that also included math.h. I still think this 
//  is better than including math.h or cmath here. I've been down that road.
//
///////////////////////////////////////////////////////////////////////////////

extern "C"
{
#ifdef _MSC_VER
  _USUL_CHECK_RETURN_ _CRTIMP int __cdecl _finite ( _USUL_IN double );
#else
  int finite ( double ) _USUL_THROW;
#endif
};


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up.
//
///////////////////////////////////////////////////////////////////////////////

#undef _USUL_THROW
#undef _USUL_IN
#undef _USUL_CHECK_RETURN_


namespace Usul {
namespace Math {


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is finite.
//
/////////////////////////////////////////////////////////////////////////////

inline bool finite ( long double v )
{
#ifdef _WIN32
  return ( 0 != ::_finite ( USUL_UNSAFE_CAST ( double, v ) ) );
#else
  return ( 0 !=  ::finite ( USUL_UNSAFE_CAST ( double, v ) ) );
#endif
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is finite.
//
/////////////////////////////////////////////////////////////////////////////

inline bool finite ( double v )
{
#ifdef _WIN32
  return ( 0 != ::_finite ( v ) );
#else
  return ( 0 !=  ::finite ( v ) );
#endif
}


/////////////////////////////////////////////////////////////////////////////
//
//  See if the number is finite.
//
/////////////////////////////////////////////////////////////////////////////

inline bool finite ( float v )
{
#ifdef _WIN32
  return ( 0 != ::_finite ( USUL_UNSAFE_CAST ( double, v ) ) );
#else
  return ( 0 !=  ::finite ( USUL_UNSAFE_CAST ( double, v ) ) );
#endif
}


///////////////////////////////////////////////////////////////////////////////
///
///  Macro for defining the finite function for 4D vector types.
///
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_FINITE_VEC4(vector_type) \
namespace Usul { namespace Math { bool finite ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::finite ( v[0] ) && \
    Usul::Math::finite ( v[1] ) && \
    Usul::Math::finite ( v[2] ) && \
    Usul::Math::finite ( v[3] ) ); \
} }; }


} // namespace Math
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
///
///  Macro for defining the finite function for 3D vector types.
///
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_FINITE_VEC3(vector_type) \
namespace Usul { namespace Math { bool finite ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::finite ( v[0] ) && \
    Usul::Math::finite ( v[1] ) && \
    Usul::Math::finite ( v[2] ) ); \
} }; }


///////////////////////////////////////////////////////////////////////////////
///
///  Macro for defining the finite function for 2D vector types.
///
///////////////////////////////////////////////////////////////////////////////

#define USUL_DECLARE_FINITE_VEC2(vector_type) \
namespace Usul { namespace Math { bool finite ( const vector_type &v ) \
{ \
  return ( \
    Usul::Math::finite ( v[0] ) && \
    Usul::Math::finite ( v[1] ) ); \
} }; }


#endif // _USUL_MATH_FINITE_FUNCTION_H_
