
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Type wrapper. Inspired by Loki::Type2Type from Andrei Alexandrescu.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_WRAPPER_H_
#define _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_WRAPPER_H_


namespace GN {
namespace MPL {


template < class T > struct TypeWrap
{
  typedef T Type;
};


}; // namespace MPL
}; // namespace GN


#endif // _GENERIC_NURBS_LIBRARY_META_PROGRAMMING_TYPE_WRAPPER_H_
