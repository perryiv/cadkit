
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class that determines if the file can be written to.
//  TODO, figure out how to do this without opening/closing the file.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Predicates/CanWrite.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/File/Remove.h"

#include <fstream>

#ifdef _MSC_VER
# include "io.h"
# define TEST_ACCESS _access
#else
# include <unistd.h>
# define TEST_ACCESS access
#endif

using namespace Usul::Predicates;


///////////////////////////////////////////////////////////////////////////////
//
//  Test function.
//
///////////////////////////////////////////////////////////////////////////////

bool CanWrite::test ( const std::string &s )
{
  // Initialize the result.
  bool result ( false );

  // If the file does not exist then create it.
  if ( false == Usul::Predicates::FileExists() ( s ) )
  {
    std::ofstream file ( s.c_str() );
    result = file.is_open();
    file.close();
    Usul::File::remove ( s );
  }

  // Otherwise, check existing file.
  else
  {
    const int write ( 2 );
    result = ( 0 == TEST_ACCESS ( s.c_str(), write ) );
  }

  // Return result.
  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Overloaded operator.
//
///////////////////////////////////////////////////////////////////////////////

bool CanWrite::operator () ( const std::string &s ) const
{
  return CanWrite::test ( s );
}
