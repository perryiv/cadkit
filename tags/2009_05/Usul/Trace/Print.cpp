
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class for printing a string.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Trace/Print.h"

#include <vector>

using namespace Usul::Trace;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize variables with file scope.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Trace
  {
    namespace Detail
    {
      std::ostream *_stream ( 0x0 );

      bool _printing ( true );
      typedef std::pair < bool, std::string > Filter;
      typedef std::vector < Filter > Filters;
      Filters _filters;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the string through the filters. Since this function just reads the 
//  container, it should be re-entrant.
//
///////////////////////////////////////////////////////////////////////////////

namespace Usul
{
  namespace Trace
  {
    namespace Detail
    {
      inline bool match ( const std::string &filter, const std::string &str )
      {
        // See if they're equal. This will handle empty strings too.
        if ( filter == str )
        {
          return true;
        }

        // Can we find the filter in the given string?
        const std::string::size_type position ( str.find ( filter ) );
        const bool found ( position != std::string::npos );
        return found;
      }

      inline bool shouldPrint ( const std::string &str )
      {
        // Loop through the filters.
        for ( Filters::const_iterator i = _filters.begin(); i != _filters.end(); ++i )
        {
          // Get filter.
          const Filter &filter ( *i );

          // See if they're equal. This will handle empty strings too.
          if ( true == match ( filter.second, str ) )
          {
            // Return the printing state for this filter.
            return filter.first;
          }
        }

        // If we get to here then use default behavior.
        return _printing;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the string.
//
///////////////////////////////////////////////////////////////////////////////

void Print::execute ( const std::string &s )
{
  if ( 0x0 != Detail::_stream )
  {
    if ( false == s.empty() )
    {
      if ( true == Detail::shouldPrint ( s ) )
      {
        *Detail::_stream << s << std::flush;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the string.
//
///////////////////////////////////////////////////////////////////////////////

void Print::execute ( const char *s )
{
  if ( 0x0 != Detail::_stream )
  {
    if ( 0x0 != s )
    {
      if ( true == Detail::shouldPrint ( s ) )
      {
        *Detail::_stream << s << std::flush;
      }
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Macro to print the number.
//
///////////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_NUMBER_PRINTING_MEMBER(number_type)\
void Print::execute ( number_type num )\
{\
  if ( 0x0 != Detail::_stream )\
  {\
    *Detail::_stream << num << std::flush;\
  }\
}


///////////////////////////////////////////////////////////////////////////////
//
//  Print the numbers.
//
///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_NUMBER_PRINTING_MEMBER ( const void * );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned long );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned short );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( unsigned int );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( long );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( short );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( int );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( double );
IMPLEMENT_NUMBER_PRINTING_MEMBER ( float );


///////////////////////////////////////////////////////////////////////////////
//
//  Set the stream to print to. 
//  Not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Print::stream ( std::ostream *s )
{
  Detail::_stream = s;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a filter. With no filters everything gets through.
//  This function is not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Print::addFilter ( bool print, const std::string &filter )
{
  if ( false == filter.empty() )
  {
    Detail::_filters.push_back ( Detail::Filter ( print, filter ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the printing state. This function is not thread safe!
//
///////////////////////////////////////////////////////////////////////////////

void Print::printing ( bool state )
{
  Detail::_printing = state;
}
