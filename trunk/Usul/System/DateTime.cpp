
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

#include <ctime>


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
