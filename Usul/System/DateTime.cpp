
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class the returns time nicely formatted dates and times.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/System/DateTime.h"
#include "Usul/Convert/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"

#include <ctime>
#include <cstring>
#include <map>


///////////////////////////////////////////////////////////////////////////////
//
//  Returns current date and time in a formatted string.
//  Maintained for backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  typedef std::map<std::string,unsigned int> Months;

  Months _months()
  {
    const unsigned int numMonths ( 12 );
    const std::string names[numMonths] = { "jan", "feb", "mar", "apr", "jun", "jul", "aug", "sep", "oct", "nov", "dec" };

    Months months;

    for ( unsigned int i = 0; i < numMonths; ++i )
    {
      months[names[i]] = i + 1;
    }

    return months;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns current date and time in a formatted string.
//  Maintained for backward compatability.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::DateTime::now() 
{
  return Usul::System::DateTime::format();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns current date and time in a formatted string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::DateTime::format ( const std::string &f ) 
{
  return Usul::System::DateTime::format ( ::time ( 0x0 ), f );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns given date and time in a formatted string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::DateTime::format ( time_t t, const std::string &f )
{
  // Determine format string.
  std::string fs ( ( true == f.empty() ) ? "%a %Y-%b-%d %I:%M:%S %p" : f );

  // Convert the time to a string.
  const unsigned int size ( 1024 );
  char buffer[size];
  ::memset ( buffer, '\0', size );
  ::tm lt ( Usul::System::DateTime::local ( t ) );
  ::strftime ( buffer, size - 1, f.c_str(), &lt );

  // Return the string.
  return std::string ( buffer );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns local time.
//
///////////////////////////////////////////////////////////////////////////////

::tm Usul::System::DateTime::local ( time_t t )
{
  ::tm lt;
  ::memset ( &lt, 0, sizeof ( ::tm ) );

#if ( defined ( _MSC_VER ) && ( _MSC_VER >= 1400 ) )

  if ( 0 != ::localtime_s ( &lt, &t ) )
    throw std::runtime_error ( "Error 2825662448: failed to get local time" );
  return lt;

#else

  ::tm *temp ( ::localtime ( &t ) );
  if ( 0x0 != temp )
    lt = *temp;
  return lt;

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Returns local time.
//
///////////////////////////////////////////////////////////////////////////////

::tm Usul::System::DateTime::local()
{
  return Usul::System::DateTime::local ( ::time ( 0x0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Pass __DATE__ macro to get YYYY.MM.DD formatted string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Usul::System::DateTime::version ( const std::string &date, const std::string &delim )
{
  // If the string is not this size then the logic below does not work.
  if ( 11 != date.size() )
    return date;

  // Get map of months.
  Detail::Months months ( Detail::_months() );

  // Get year.
  const std::string year  ( date.begin() + 7, date.end() );

  // Get the month. Prepend with zero if necessary.
  std::string month ( Usul::Strings::lowerCase ( std::string ( date.begin() + 0, date.begin() + 3 ) ) );
  month = Usul::Convert::Type<unsigned int,std::string>::convert ( months[month] );
  month = ( ( 1 == month.size() ) ? Usul::Strings::format ( "0", month ) : month );

  // Get day of the month. Prepend with zero if necessary.
  std::string day ( date.begin() + 4, date.begin() + 6 );
  if ( ' ' == day.at(0) )
    day.at(0) = '0';

  // Put together and return the final string.
  const std::string result ( Usul::Strings::format ( year, delim, month, delim, day ) );
  return result;
}
