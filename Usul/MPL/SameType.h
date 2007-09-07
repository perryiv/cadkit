
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

#ifndef _USUL_MPL_SAME_TYPE_H_
#define _USUL_MPL_SAME_TYPE_H_

#include "Usul/MPL/StaticAssert.h"
#include "Usul/MPL/TypeWrap.h"


namespace Usul {
namespace MPL {


///////////////////////////////////////////////////////////////////////////////
//
//  Class for determining if the two template parameters are the same type.
//  Inspired by Loki::IsEqualType from Andrei Alexandrescu.
//
///////////////////////////////////////////////////////////////////////////////


#if defined ( _MSC_VER ) && _MSC_VER < 1300


template < class T, class U > class IsSameType
{
  struct Yes{};
  struct No { char dummy[10]; };

	static Yes _test ( TypeWrap<T> );
	static No  _test ( ... );

public:

  enum { RESULT = ( sizeof ( _test ( TypeWrap<U>() ) ) == sizeof ( Yes ) ) };
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


} // namespace MPL
} // namespace Usul


///////////////////////////////////////////////////////////////////////////////
//
//  Handy macro for using above class.
//
///////////////////////////////////////////////////////////////////////////////

#define USUL_ASSERT_SAME_TYPE(Type1,Type2)\
  USUL_STATIC_ASSERT ( ( 0 != ::Usul::MPL::IsSameType<Type1,Type2>::RESULT ) )


#endif // _USUL_MPL_SAME_TYPE_H_
