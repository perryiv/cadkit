///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/CommandLine/Options.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace Usul::CommandLine;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Options::Options() : _options()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert given option and value.
//
///////////////////////////////////////////////////////////////////////////////

void Options::insert ( const std::string& option, const std::string& value )
{
  _options.insert( Map::value_type ( option, value ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this option exist?
//
///////////////////////////////////////////////////////////////////////////////

bool Options::option( const std::string& option ) const
{
  Map::const_iterator iter = _options.find( option );
  return ( iter != _options.end() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the value for the given option.
//
///////////////////////////////////////////////////////////////////////////////

std::string Options::value ( const std::string& option ) const
{
  Map::const_iterator iter = _options.find( option );
  if( iter != _options.end() )
    return iter->second;
  return "";
}


// -----------------------------------------------------------------------------
// The value as a string
// -----------------------------------------------------------------------------
std::string Options::stringValue ( const std::string& option ) const
{
  Map::const_iterator iter = _options.find( option );
  if( iter != _options.end() )
    return iter->second;
  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
// The value as an integer
//
///////////////////////////////////////////////////////////////////////////////
 
unsigned long int Options::intValue (const std::string& option) const
{
  unsigned long int value ( 0 );
  std::istringstream in ( this->stringValue(option) );
  in >> value;
  return value;
}

///////////////////////////////////////////////////////////////////////////////
//
// Returns a float value from the Options
//
///////////////////////////////////////////////////////////////////////////////

float Options::floatValue(const std::string& option) const
{
  float value ( 0 );
  std::istringstream in ( this->stringValue(option) );
  in >> value;
  return value;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Populates with values from a stream.
//
///////////////////////////////////////////////////////////////////////////////

std::istream&  Usul::CommandLine::operator>>( std::istream& in, Options& o )
{
  const unsigned int size ( 1024 );

  while( !in.eof() )
  {
    std::vector< char > line ( size );

    in.getline( &line.front(), line.size() );

    std::string data ( &line.front() );

    // Find the first space.
    std::string::size_type position ( data.find( ' ' ) );

    if( position != std::string::npos )
    {
      std::string first ( data.begin(), data.begin() + position );
      std::string second ( data.begin() + position, data.end() );

      o.insert ( first, second );
    }
  }
 
  return in;
}

