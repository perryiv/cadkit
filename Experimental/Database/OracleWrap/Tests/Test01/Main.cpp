
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for OracleWrap.
//
///////////////////////////////////////////////////////////////////////////////

#include "OracleWrap/Database.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Strings/Format.h"
#include "Usul/System/LastError.h"

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <fstream>
#include <iostream>
#include <vector>

namespace Helper {


///////////////////////////////////////////////////////////////////////////////
//
//  Get the input data.
//
///////////////////////////////////////////////////////////////////////////////

void getTestData ( std::string &database, std::string &user, std::string &password, std::string &sql )
{
  Usul::CommandLine::Arguments::Args args ( Usul::CommandLine::Arguments::instance().args() );
  if ( args.size() < 2 )
    throw std::runtime_error ( Usul::Strings::format ( __FILE__, ", ", __LINE__ ) );

  const std::string file ( args.at ( 1 ) );
  std::ifstream in ( file.c_str() );
  if ( false == in.is_open() )
    throw std::runtime_error ( Usul::Strings::format ( __FILE__, ", ", __LINE__ ) );

  in >> database >> user >> password;

  std::vector<char> line ( 1024, '\0' );
  in.getline ( &line[0], line.size() - 1 ); // Gets us to the beginning of the next line.
  in.getline ( &line[0], line.size() - 1 ); // Get the data.
  sql = &line[0];
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void run()
{
  // Initialize.
  Usul::System::LastError::init();

  // Get information.
  std::string database, user, password, sql;
  Helper::getTestData ( database, user, password, sql );

  // Make a database connection.
  OracleWrap::Database::RefPtr db ( new OracleWrap::Database ( database, user, password ) );

  // Make a query.
  OracleWrap::Result::RefPtr result ( db->execute ( sql ) );

  // Get the number of columns.
  const unsigned int numColumns ( result->numColumns() );

  // Loop through the result.
  while ( result->prepareNextRow() )
  {
    std::string value;
    for ( unsigned int i = 0; i < numColumns; ++i )
    {
      *result >> value;
      std::cout << ( ( i > 0 ) ? ", " : "" ) << value;
    }
    std::cout << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to handle Windows structured exception.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

int handleStructuredException ( DWORD code )
{
  const std::string message ( Usul::System::LastError::message ( code ) );
  std::cout << Usul::Strings::format ( 
    "Error 2917508413: structured exception ", code, " generated. ", 
    ( ( false == message.empty() ) ? message : "" ) ) << std::endl;
  return EXCEPTION_EXECUTE_HANDLER;
}

#endif

} // namespace Helper


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::CommandLine::Arguments::instance().set ( argc, argv );
#ifdef _MSC_VER
  __try 
  {
#endif
  Usul::Functions::safeCall ( &Helper::run, "3715029360" );
#ifdef _MSC_VER
  }
  __except ( Helper::handleStructuredException ( ::GetExceptionCode() ) )
  {
    return 1;
  }
#endif
  return 0;
}
