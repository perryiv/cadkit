
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Date class wrapper around boost's date class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_DATE_H__
#define __OSGTOOLS_ANIMATE_DATE_H__

#include "OsgTools/Export.h"

#include <string>

#ifdef _MSC_VER
# pragma warning ( disable : 4996 )
# pragma warning ( disable : 4561 )
#endif

#include "boost/date_time/gregorian/gregorian.hpp"
//#include "boost/date_time/gregorian/greg_serialize.hpp"

namespace boost { namespace serialization { class access; } }

namespace OsgTools {
namespace Animate {

class OSG_TOOLS_EXPORT Date
{
public:

  Date();

  Date ( const std::string& date );
  Date ( boost::date_time::special_values value );
  Date ( const boost::gregorian::date& date );

  /// Get the underlying boost date.
  boost::gregorian::date&           date();
  const boost::gregorian::date&     date() const;

  /// Get the day.
  unsigned int day() const;

  /// Get the month.
  unsigned int month() const;

  /// Get the year.
  unsigned int year() const;

  /// Increment date by one day.
  void         increment();

  void         incrementDay();
  void         incrementMonth();
  void         incrementYear();

  void         moveBackNumDays ( unsigned int );

  std::string  toString() const;
  void         fromString( const std::string& date );

  bool operator<( const Date& rhs ) const;
  bool operator>( const Date& rhs ) const;

  bool operator==( const Date& rhs ) const;
  bool operator!=(const Date& rhs ) const;

  bool operator<=( const Date& rhs ) const;
  bool operator>=( const Date& rhs ) const;

protected:
  long _toJulian() const;

private:
  friend class boost::serialization::access;
  template < class Archive > void serialize( Archive &ar, const unsigned int version );

  boost::gregorian::date _date;
};

template < class Archive >
void Date::serialize( Archive &ar, const unsigned int version )
{
  //ar & boost::serialization::make_nvp( "Date", _date );
}


inline std::ostream& operator<<( std::ostream& out, const Date& date )
{
  out << date.toString();
  return out;
}

inline std::istream& operator>> ( std::istream& in, Date& date )
{
  std::string s;
  in >> s;
  date.fromString( s );
  return in;
}

}
}

#endif // __OSGTOOLS_ANIMATE_DATE_H__
