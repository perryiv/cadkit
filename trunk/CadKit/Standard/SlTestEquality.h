
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  SlTestEquality: Functions for testing equality.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_STANDARD_LIBRARY_TEST_EQUALITY_FUNCTIONS_H_
#define _CADKIT_STANDARD_LIBRARY_TEST_EQUALITY_FUNCTIONS_H_


namespace CadKit
{
///////////////////////////////////////////////////////////////////////////////
//
//  Macro for declaring explicit functions. If these are templated with <T>
//  then VC++ gets confused and uses these when it should be using the 
//  equivalent functions in SlVecX.h.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_EQUALITY(T) \
inline bool isEqual ( const T &a, const T &b ) \
{ \
  return ( a == b ); \
} \
template<class I> inline bool isEqualArray ( const T *a, const T *b, const I &size ) \
{ \
  for ( I i = 0; i < size; ++i ) \
    if ( a[i] != b[i] ) \
      return false; \
  return true; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Similar to above but for floats.
//
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_EQUALITY_WITHIN_TOLERANCE(T) \
CADKIT_DECLARE_EQUALITY(T) \
inline bool isEqual ( const T &a, const T &b, const T &tol ) \
{ \
  return ( a < b ) ? ( b - a ) < tol : ( a - b ) < tol; \
} \
template<class I> inline bool isEqualArray ( const T *a, const T *b, const I &size, const T &tol ) \
{ \
  for ( I i = 0; i < size; ++i ) \
    if ( false == CadKit::isEqual ( a[i], b[i], tol ) ) \
      return false; \
  return true; \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Explicit declarations.
//
///////////////////////////////////////////////////////////////////////////////

CADKIT_DECLARE_EQUALITY ( unsigned int );
CADKIT_DECLARE_EQUALITY ( unsigned short );
CADKIT_DECLARE_EQUALITY ( unsigned long );
CADKIT_DECLARE_EQUALITY ( int );
CADKIT_DECLARE_EQUALITY ( short );
CADKIT_DECLARE_EQUALITY ( long );
CADKIT_DECLARE_EQUALITY_WITHIN_TOLERANCE ( long double );
CADKIT_DECLARE_EQUALITY_WITHIN_TOLERANCE ( double );
CADKIT_DECLARE_EQUALITY_WITHIN_TOLERANCE ( float );


}; // namespace CadKit.


#endif // _CADKIT_STANDARD_LIBRARY_TEST_EQUALITY_FUNCTIONS_H_
