
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Compile-time type check.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_SAME_TYPE_H_
#define _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_SAME_TYPE_H_

#include "GN/MPL/StaticAssert.h"
#include "GN/MPL/TypeWrap.h"


namespace GN {
namespace MPL {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for determining if the two template parameters are the same type.
//  Inspired by Loki::IsEqualType from Andrei Alexandrescu.
//
///////////////////////////////////////////////////////////////////////////////


#ifdef _MSC_VER


template < class T, class U > class IsSameType
{
  struct Yes{};
  struct No { char dummy[10]; };

	static Yes _test ( GN::MPL::TypeWrap<T> );
	static No  _test ( ... );

public:

  enum { RESULT = ( sizeof ( _test ( GN::MPL::TypeWrap<U>() ) ) == sizeof ( Yes ) ) };
};


#else


template < class T, class U > struct IsSameType
{
  enum { RESULT = 0 };
};
template < class T > struct IsSameType < T, T >
{
  enum { RESULT = 1 };
};


#endif


}; // namespace GN
}; // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro for using above class.
//
///////////////////////////////////////////////////////////////////////////////

#define GN_ASSERT_SAME_TYPE(Type1,Type2)\
  GN_STATIC_ASSERT ( ( 0 != ::GN::MPL::IsSameType<Type1,Type2>::RESULT ) )


#endif // _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_SAME_TYPE_H_
