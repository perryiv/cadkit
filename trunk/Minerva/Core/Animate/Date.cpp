
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

#include "Usul/Convert/Convert.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Format.h"
#include "Usul/Strings/Split.h"

#include "boost/algorithm/string/erase.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/date_time/local_time/local_date_time.hpp"
#include "boost/date_time/local_time/local_time_types.hpp"
#include "boost/date_time/local_time/posix_time_zone.hpp"
#include "boost/date_time/time_zone_base.hpp"

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

Date::Date ( boost::date_time::special_values value ) :
  _date ( value )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date ( const boost::gregorian::date& date ) :
  _date ( date )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Date::Date ( const boost::posix_time::ptime& date ) :
  _date ( date )
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
  return _date.date().day();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the month.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Date::month() const
{
  return _date.date().month();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the year.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Date::year() const
{
  return _date.date().year();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Increment date by given increment type and amount.
//
///////////////////////////////////////////////////////////////////////////////

void Date::increment ( IncrementType type, long amount )
{
  switch ( type )
  {
  case Date::INCREMENT_SECOND:
    _date = _date + boost::posix_time::time_duration ( boost::posix_time::seconds ( amount ) );
    break;
  case Date::INCREMENT_MINUTE:
    _date = _date + boost::posix_time::time_duration ( boost::posix_time::minutes ( amount ) );
    break;
  case Date::INCREMENT_HOUR:
    _date = _date + boost::posix_time::time_duration ( boost::posix_time::hours ( amount ) );
    break;
  case Date::INCREMENT_DAY:
    _date = _date + boost::gregorian::date_duration ( amount );
    break;
  case Date::INCREMENT_MONTH:
    _date = _date + boost::gregorian::months ( amount );
    break;
  case Date::INCREMENT_YEAR:
    _date = _date + boost::gregorian::years ( amount );
    break;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the date as a string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Date::toString() const
{
  return boost::posix_time::to_simple_string(_date);
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

  _date = boost::posix_time::time_from_string ( date );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date less than this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator<( const Date& rhs ) const
{
  return _date < rhs._date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date greater than this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator>( const Date& rhs ) const
{
  return _date > rhs._date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the two dates equal?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator==( const Date& rhs ) const
{
  return _date == rhs._date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are the two dates different?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator!=(const Date& rhs ) const
{
  return _date != rhs._date;
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
  return _date <= rhs._date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the given date the same or after this date?
//
///////////////////////////////////////////////////////////////////////////////

bool Date::operator>=( const Date& rhs ) const
{
  return _date >= rhs._date;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the underlying boost date.
//
///////////////////////////////////////////////////////////////////////////////

boost::gregorian::date Date::date() const
{
  return _date.date();
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


///////////////////////////////////////////////////////////////////////////////
//
//  Create date from kml.
//
///////////////////////////////////////////////////////////////////////////////

Date::DateType Date::createFromKml ( const std::string& value )
{
  typedef std::vector<std::string> Strings;
  Strings strings;
  Usul::Strings::split ( value, "T", false, strings );
  
  std::string date ( strings.size() > 0 ? strings[0] : "" );
  
  // Currently not handling time.
  std::string time ( strings.size() > 1 ? strings[1] : "" );
  
  Strings parts;
  Usul::Strings::split ( date, "-", false, parts );
  
  if ( false == parts.empty() )
  {
    int year ( Usul::Convert::Type<std::string,int>::convert ( parts[0] ) );
    const int month ( parts.size() > 1 ? Usul::Convert::Type<std::string,int>::convert ( parts[1] ) : 1 );
    const int day ( parts.size() > 2 ? Usul::Convert::Type<std::string,int>::convert ( parts[2] ) : 1 );
    
    if ( year < 1400 )
      year += 1400;
    
    // The date.
    boost::gregorian::date date ( boost::gregorian::from_simple_string ( Usul::Strings::format ( year, "-", month, "-", day ) ) );

    // The time duration.
    boost::posix_time::time_duration timeDuration ( boost::posix_time::hours ( 0 ) );

    // The timezone.
    boost::local_time::time_zone_ptr timeZone;

    if ( false == time.empty() )
    {
      boost::algorithm::erase_all ( time, "Z" );

      // See if there is a plus or negative.
      if ( time.size() > 8 && ( '-' == time[8] || '+' == time[8] ) )
      {
        std::string zone ( time.begin() + 8, time.end() );
        const std::string temp ( time );
        time.assign ( temp.begin(), temp.begin() + 8 );
        
        typedef Usul::Convert::Type<std::string,int> ToInt;
        int offset ( ToInt::convert ( zone ) );
        int hourOffset ( offset / 100 );
        
        // Offset from UTC.
        boost::posix_time::time_duration utcOffset ( hourOffset, 0, 0 );
        
        // Daylight savings offsets.  TODO: Find out if the RSS feed will have accounted for dst.
        boost::local_time::dst_adjustment_offsets dstOffsets ( boost::posix_time::time_duration ( 0, 0, 0 ),
                                                              boost::posix_time::time_duration ( 0, 0, 0 ),
                                                              boost::posix_time::time_duration ( 0, 0, 0 ) );
        
        boost::shared_ptr<boost::local_time::dst_calc_rule> rules;
        boost::local_time::time_zone_names names ( "", "", "", "" );
        
        timeZone = boost::local_time::time_zone_ptr ( new boost::local_time::custom_time_zone ( names, utcOffset, dstOffsets, rules ) );
      }

      parts.clear();
      Usul::Strings::split ( time, ":", false, parts );

      // We should have 3 strings.
      if ( 3 == parts.size() )
      {
        const int hours   ( Usul::Convert::Type<std::string,int>::convert ( parts[0] ) );
        const int minutes ( parts.size() > 1 ? Usul::Convert::Type<std::string,int>::convert ( parts[1] ) : 1 );
        const int seconds ( parts.size() > 2 ? Usul::Convert::Type<std::string,int>::convert ( parts[2] ) : 1 );

        timeDuration = boost::posix_time::time_duration ( hours, minutes, seconds );
      }
    }
    
    return boost::posix_time::ptime ( boost::local_time::local_date_time ( date, timeDuration, timeZone, true ).utc_time() );
  }
  
  return boost::posix_time::not_a_date_time;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create date from rss.
//
///////////////////////////////////////////////////////////////////////////////

Date::DateType Date::createFromRSS ( const std::string& input )
{
  try
  {
    // Make a copy to work with.
    std::string sDate ( input );
    
    // Trim white space.
    boost::algorithm::trim ( sDate );
    
    if ( false == sDate.empty() && sDate.size() > 27 )
    {
      const std::string dayOfWeek ( sDate, 0, 3 );
      const std::string day ( sDate, 5, 2 );
      const std::string month ( sDate, 8, 3 );
      const std::string year ( sDate, 12, 4 );
      const std::string hours ( sDate, 17, 2 );
      const std::string minutes ( sDate, 20, 2 );
      const std::string seconds ( sDate, 23, 2 );
      const std::string zone ( sDate, 26 ); // Get the remaining characters.
      
      // The timezone.
      boost::local_time::time_zone_ptr timeZone;
      
      typedef Usul::Convert::Type<std::string,int> ToInt;
      
      // See if the zone is an offset.
      if ( false == zone.empty() && ( '-' == zone[0] || '+' == zone[1] ) )
      {
        int offset ( ToInt::convert ( zone ) );
        int hourOffset ( offset / 100 );
        
        // Offset from UTC.
        boost::posix_time::time_duration utcOffset ( hourOffset, 0, 0 );
        
        // Daylight savings offsets.  TODO: Find out if the RSS feed will have accounted for dst.
        boost::local_time::dst_adjustment_offsets dstOffsets ( boost::posix_time::time_duration ( 0, 0, 0 ),
                                                              boost::posix_time::time_duration ( 0, 0, 0 ),
                                                              boost::posix_time::time_duration ( 0, 0, 0 ) );
        
        boost::shared_ptr<boost::local_time::dst_calc_rule> rules;
        boost::local_time::time_zone_names names ( "", "", "", "" );
        
        timeZone = boost::local_time::time_zone_ptr ( new boost::local_time::custom_time_zone ( names, utcOffset, dstOffsets, rules ) );
      }
      
      boost::posix_time::time_duration time ( ToInt::convert ( hours ), ToInt::convert ( minutes ), ToInt::convert ( seconds ) );
      boost::gregorian::date date ( boost::gregorian::from_simple_string ( year + "-" + month + "-" + day ) );
      boost::posix_time::ptime lastUpdate ( date, time );
      
      boost::posix_time::ptime utcTime ( boost::local_time::local_date_time ( date, time, timeZone, true ).utc_time() );
      return utcTime;
    }
  }
  USUL_DEFINE_SAFE_CALL_CATCH_BLOCKS ( "2928650239" );
  
  return boost::posix_time::not_a_date_time;
}
