
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Length units.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _UNITS_LIBRARY_LENGTH_H_
#define _UNITS_LIBRARY_LENGTH_H_

#include <sstream>
#include <stdexcept>


namespace Units {
namespace Length {


///////////////////////////////////////////////////////////////////////////////
//
//  Unit tokens.
//
///////////////////////////////////////////////////////////////////////////////

struct Meters{};
struct Millimeters{};
struct Centimeters{};
struct Feet{};
struct Inches{};


///////////////////////////////////////////////////////////////////////////////
//
//  Generic template. Attemps to use specializations with Meters.
//
///////////////////////////////////////////////////////////////////////////////

template < class FromType, class ToType > struct Convert
{
  template < class T > static T value ( T t )
  {
    T m  ( Convert<FromType,Meters>::value ( t ) );
    return Convert<Meters,  ToType>::value ( m );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define same-type conversions.
//
///////////////////////////////////////////////////////////////////////////////

#define DEFINE_SAME_TYPE_CONVERSION(Type)\
template <> struct Convert < Type, Type > \
{ \
  template < class T > static T value ( T t ) { return t; } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Define same-type conversions.
//
///////////////////////////////////////////////////////////////////////////////

DEFINE_SAME_TYPE_CONVERSION ( Meters );
DEFINE_SAME_TYPE_CONVERSION ( Millimeters );
DEFINE_SAME_TYPE_CONVERSION ( Centimeters );
DEFINE_SAME_TYPE_CONVERSION ( Feet );
DEFINE_SAME_TYPE_CONVERSION ( Inches );


///////////////////////////////////////////////////////////////////////////////
//
//  Done with this.
//
///////////////////////////////////////////////////////////////////////////////

#undef DEFINE_SAME_TYPE_CONVERSION


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to define type conversions.
//
///////////////////////////////////////////////////////////////////////////////

#define DEFINE_CONVERSION(FromType,ToType,conversion_constant)\
template <> struct Convert < FromType, ToType > \
{ \
  template < class T > static T value ( T t ) \
  { \
    return ( t * conversion_constant ); \
  } \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Define explicit conversions.
//
///////////////////////////////////////////////////////////////////////////////

DEFINE_CONVERSION ( Meters,      Millimeters, 1000          );
DEFINE_CONVERSION ( Millimeters, Meters,         0.001f     );
DEFINE_CONVERSION ( Meters,      Centimeters,  100          );
DEFINE_CONVERSION ( Centimeters, Meters,         0.01f      );
DEFINE_CONVERSION ( Meters,      Feet,           3.2808399f );
DEFINE_CONVERSION ( Feet,        Meters,         0.3048f    );
DEFINE_CONVERSION ( Meters,      Inches,        39.3700787f );
DEFINE_CONVERSION ( Inches,      Meters,         0.0254f    );
DEFINE_CONVERSION ( Feet,        Inches,        12          );
DEFINE_CONVERSION ( Inches,      Feet,           0.0833333f );


///////////////////////////////////////////////////////////////////////////////
//
//  Done with this.
//
///////////////////////////////////////////////////////////////////////////////

#undef DEFINE_CONVERSION


///////////////////////////////////////////////////////////////////////////////
//
//  Convert the value.
//
///////////////////////////////////////////////////////////////////////////////

template < class T > inline T convert ( const std::string &from, const std::string &to, T v )
{
  // Make a copy of the strings and convert to upper-case.
  std::string f ( from ), t ( to );
  std::transform ( f.begin(), f.end(), f.begin(), ::toupper );
  std::transform ( t.begin(), t.end(), t.begin(), ::toupper );

  // Initialize.
  T answer ( 0 );

  // Convert from given units to meters.
  if      ( "MILLIMETERS" == f || "MM" == f )
    answer = Convert<Millimeters,Meters>::value ( v );
  else if ( "CENTIMETERS" == f || "CM" == f )
    answer = Convert<Centimeters,Meters>::value ( v );
  else if ( "METERS"      == f || "M"  == f )
    answer = Convert<Meters,     Meters>::value ( v );
  else if ( "INCHES"      == f || "IN" == f )
    answer = Convert<Inches,     Meters>::value ( v );
  else if ( "FEET"        == f || "FT" == f )
    answer = Convert<Feet,       Meters>::value ( v );
  else
  {
    std::ostringstream out;
    out << "Error 1772416405: Invalid argument for 'from' unit conversion."
        << "\n\tUnits: " << from
        << "\n\tFile: " << __FILE__
        << "\n\tLine: " << __LINE__;
    throw std::invalid_argument ( out.str() );
  }

  // Convert from meters to desired units.
  if      ( "MILLIMETERS" == t || "MM" == t )
    answer = Convert<Meters,Millimeters>::value ( answer );
  else if ( "CENTIMETERS" == t || "CM" == t )
    answer = Convert<Meters,Centimeters>::value ( answer );
  else if ( "METERS"      == t || "M"  == t )
    answer = Convert<Meters,     Meters>::value ( answer );
  else if ( "INCHES"      == t || "IN" == t )
    answer = Convert<Meters,     Inches>::value ( answer );
  else if ( "FEET"        == t || "FT" == t )
    answer = Convert<Meters,     Feet  >::value ( answer );
  else
  {
    std::ostringstream out;
    out << "Error 3751287951: Invalid argument for 'to' unit conversion."
        << "\n\tUnits: " << to
        << "\n\tFile: " << __FILE__
        << "\n\tLine: " << __LINE__;
    throw std::invalid_argument ( out.str() );
  }

  // Return the answer.
  return answer;
}


}; // namespace Length
}; // namespace Units


#endif // _UNITS_LIBRARY_LENGTH_H_
