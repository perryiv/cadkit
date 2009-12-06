
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compile-time assertion.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_STATIC_ASSERT_H_
#define _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_STATIC_ASSERT_H_


namespace GN {
namespace MPL {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic declaration but no definition. Specialization for true.
//
///////////////////////////////////////////////////////////////////////////////

template < bool T > struct StaticAssert;
template <> struct StaticAssert < true >{};


}; // namespace MPL
}; // namespace GN


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro for using static assertions.
//
///////////////////////////////////////////////////////////////////////////////

#define GN_STATIC_ASSERT(exp)\
  GN::MPL::StaticAssert<(exp)>()


#endif // _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_STATIC_ASSERT_H_