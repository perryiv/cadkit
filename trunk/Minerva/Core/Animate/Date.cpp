
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

#include "Minerva/Core/Animate/Date.h"

#include "Usul/Strings/Split.h"
#include "Usul/Strings/Convert.h"

#include <vector>
#include <sstream>

using namespace Minerva::Core::Animate;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date() :
_date()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date( const std::string& date ) :
_date ( boost::gregorian::from_simple_string ( date ) )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date ( boost::date_time::special_values value ) : _date ( value )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date ( const boost::gregorian::date& date ) : _date ( date )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the day.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Date::day() const
{
  return _date.day();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the month.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Date::month() const
{
  return _date.month();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the year.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Date::year() const
{
  return _date.year();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the day.
//
///////////////////////////////////////////////////////////////////////////////

void Date::increment()
{
  boost::gregorian::date_duration dd(1);
  _date = _date + dd;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the date as a string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Date::toString() const
{
  return boost::gregorian::to_simple_string(_date);
}


///////////////////////////////////////////////////////////////////////////////
//
//  construct the date from a string.
//
///////////////////////////////////////////////////////////////////////////////

void Date::fromString( const std::string& date )
{
  USUL_TRACE_SCOPE;
  USUL_TRACE_1 ( date );

  _date = boost::gregorian::from_simple_string ( date );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date less than this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator<( const Date& rhs ) const
{
  return this->_toJulian() < rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date greater than this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator>( const Date& rhs ) const
{
  return this->_toJulian() > rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the two dates equal?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator==( const Date& rhs ) const
{
  return this->_toJulian() == rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the two dates different?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator!=(const Date& rhs ) const
{
  return this->_toJulian() != rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Move the date back the given number of days.
//
///////////////////////////////////////////////////////////////////////////////

void Date::moveBackNumDays ( unsigned int days )
{
  _date = _date - boost::gregorian::date_duration( days );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date the same or before this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator<=( const Date& rhs ) const
{
  return this->_toJulian() <= rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date the same or after this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator>=( const Date& rhs ) const
{
  return this->_toJulian() >= rhs._toJulian();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get this date's Julian day value.
//
///////////////////////////////////////////////////////////////////////////////

long Date::_toJulian() const
{
  return _date.julian_day();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the underlying boost date.
//
///////////////////////////////////////////////////////////////////////////////

boost::gregorian::date& Date::date()
{
  return _date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the underlying boost date.
//
///////////////////////////////////////////////////////////////////////////////

const boost::gregorian::date& Date::date() const
{
  return _date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the day by one.
//
///////////////////////////////////////////////////////////////////////////////

void Date::incrementDay()
{
  boost::gregorian::date_duration dd(1);
  _date = _date + dd;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the month by one.
//
///////////////////////////////////////////////////////////////////////////////

void Date::incrementMonth()
{
  boost::gregorian::months one(1);
  _date = _date + one;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment the year by one.
//
///////////////////////////////////////////////////////////////////////////////

void Date::incrementYear()
{
  boost::gregorian::years one(1);
  _date = _date + one;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper struct to convert string to int for a given month name.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  struct MonthToInt
  {
    typedef std::map < std::string, int > Months;

    MonthToInt () : _months()
    {
      _months.insert ( Months::value_type ( "jan", 1 ) );
      _months.insert ( Months::value_type ( "feb", 2 ) );
      _months.insert ( Months::value_type ( "mar", 3 ) );
      _months.insert ( Months::value_type ( "apr", 4 ) );
      _months.insert ( Months::value_type ( "may", 5 ) );
      _months.insert ( Months::value_type ( "jun", 6 ) );
      _months.insert ( Months::value_type ( "jul", 7 ) );
      _months.insert ( Months::value_type ( "aug", 8 ) );
      _months.insert ( Months::value_type ( "sep", 9 ) );
      _months.insert ( Months::value_type ( "oct", 10 ) );
      _months.insert ( Months::value_type ( "nov", 11 ) );
      _months.insert ( Months::value_type ( "dec", 12 ) );
    }

    int operator () ( const std::string& s )
    {
      std::string month ( s );
      std::transform ( month.begin(), month.end(), month.begin(), ::tolower );

      return _months[month];
    }

  private:
    Months _months;
  };

  MonthToInt monthToInt;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the date to stream.
//
///////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<( std::ostream& out, const Date& date )
{
  USUL_TRACE_SCOPE_STATIC;

  out << date.toString();
  return out;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the date from stream.
//
///////////////////////////////////////////////////////////////////////////////

std::istream& operator>> ( std::istream& in, Date& date )
{
  USUL_TRACE_SCOPE_STATIC;

  std::string s;
  in >> s;

  // Get the 3 components of the month.
  std::vector < std::string > strings;
  Usul::Strings::split ( s, '-', false, strings );

  if ( 3 == strings.size() )
  {
    // Year.
    int year ( Usul::Strings::fromString <int> ( strings[0] ) );

    // Month.
    int month ( Detail::monthToInt ( strings[1] ) );

    // Day
    int day ( Usul::Strings::fromString <int> ( strings[2] ) );

    date = Date ( boost::gregorian::date ( year, month, day ) );
  }
  return in;
}
