
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Date class wrapper around boost's date class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ANIMATE_DATE_H__
#define __MINERVA_CORE_ANIMATE_DATE_H__

#include "Minerva/Core/Export.h"

#include "Usul/Trace/Trace.h"

#include <string>

#ifdef _MSC_VER
# pragma warning ( disable : 4996 )
# pragma warning ( disable : 4561 )
#endif

#include "boost/date_time/gregorian/gregorian.hpp"

namespace Minerva {
namespace Core {
namespace Animate {

class MINERVA_EXPORT Date
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
  boost::gregorian::date _date;
};

}
}
}

std::ostream& operator<<( std::ostream& out, const Minerva::Core::Animate::Date& date );
std::istream& operator>> ( std::istream& in, Minerva::Core::Animate::Date& date );

#endif // __MINERVA_CORE_ANIMATE_DATE_H__
