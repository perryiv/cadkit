
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  String formatting functions.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_STRINGS_FORMAT_H_
#define _USUL_STRINGS_FORMAT_H_

#include "Usul/Headers/SStream.h"

namespace Usul {
namespace Strings {


///////////////////////////////////////////////////////////////////////////////
//
//  Format the string.
//
///////////////////////////////////////////////////////////////////////////////

template < class T1 > 
inline std::string format ( const T1 &t1 )
{
  std::ostringstream out;
  out << t1;
  return out.str();
}
template < class T1, class T2 > 
inline std::string format ( const T1 &t1, const T2 &t2 )
{
  std::ostringstream out;
  out << t1 << t2;
  return out.str();
}
template < class T1, class T2, class T3 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3 )
{
  std::ostringstream out;
  out << t1 << t2 << t3;
  return out.str();
}
template < class T1, class T2, class T3, class T4 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25 > 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25;
  return out.str();
}  
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26> 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25, const T26 &t26 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25 << t26;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27> 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25, const T26 &t26, const T27 &t27 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25 << t26 << t27;
  return out.str();
}
template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28> 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25, const T26 &t26, const T27 &t27, const T28 &t28 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25 << t26 << t27 << t28;
  return out.str();
}

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29> 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25, const T26 &t26, const T27 &t27, const T28 &t28, const T29 &t29 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25 << t26 << t27 << t28 << t29;
  return out.str();
} 

template < class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30> 
inline std::string format ( const T1 &t1, const T2 &t2, const T3 &t3, const T4 &t4, const T5 &t5, const T6 &t6, const T7 &t7, const T8 &t8, const T9 &t9, const T10 &t10, const T11 &t11, const T12 &t12, const T13 &t13, const T14 &t14, const T15 &t15, const T16 &t16, const T17 &t17, const T18 &t18, const T19 &t19, const T20 &t20, const T21 &t21, const T22 &t22, const T23 &t23, const T24 &t24, const T25 &t25, const T26 &t26, const T27 &t27, const T28 &t28, const T29 &t29, const T30 &t30 )
{
  std::ostringstream out;
  out << t1 << t2 << t3 << t4 << t5 << t6 << t7 << t8 << t9 << t10 << t11 << t12 << t13 << t14 << t15 << t16 << t17 << t18 << t19 << t20 << t21 << t22 << t23 << t24 << t25 << t26 << t27 << t28 << t29 << t30;
  return out.str();
} 
///////////////////////////////////////////////////////////////////////////////
//
//  Format date string. Pass __DATE__ macro.
//
///////////////////////////////////////////////////////////////////////////////

inline std::string formatDate ( const std::string &d )
{
  // If the string is not this size then the logic below does not work.
  if ( 11 != d.size() )
    return d;

  const std::string year  ( d.begin() + 7, d.end() );
  const std::string month ( d.begin() + 0, d.begin() + 3 );
  const std::string day   ( d.begin() + 4, d.begin() + 6 );

  std::ostringstream out;
  out << day << '-' << month << '-' << year;

  return out.str();
}


} // namespace Strings
} // namespace Usul


#endif // _USUL_STRINGS_FORMAT_H_
