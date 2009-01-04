
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
//  Test program for SQLiteWrap.
//
///////////////////////////////////////////////////////////////////////////////

#include "Database/SQLite/Connection.h"
#include "Database/SQLite/Result.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Print the result.
//
///////////////////////////////////////////////////////////////////////////////

void _printResult ( CadKit::Database::SQLite::Result::RefPtr result )
{
  if ( false == result.valid() )
    return;

  const unsigned int numColumns ( result->numColumns() );
  if ( 0 == numColumns )
    return;

  std::cout << result->columnName ( 0 );
  for ( unsigned int j = 1; j < numColumns; ++j )
  {
    std::cout << ", " << result->columnName ( j );
  }
  std::cout << '\n';

  unsigned int row ( 0 );
  while ( true == result->prepareNextRow() )
  {
    std::cout << row++;
    for ( unsigned int i = 0; i < numColumns; ++i )
    {
      std::string value;
      *result >> value;
      std::cout << ", " << value;
    }
    std::cout << std::endl;
  }
  std::cout << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run a test.
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  typedef CadKit::Database::SQLite::Connection Connection;
  typedef CadKit::Database::SQLite::Result Result;

  const std::string file ( "test.sqlite" );
  Usul::Scope::RemoveFile removeFile ( file );
  Usul::File::remove ( file );
  Connection::ValidAccessRefPtr db ( new Connection ( file ) );

  const std::string table ( "Array" );
  const std::string column0 ( "MyIndex" );
  const std::string column1 ( "Data" );
  const std::string column2 ( "Square" );

  db->execute ( Usul::Strings::format ( "create table ", table, " ( ", column0, " integer, ", column1, " double )" ) );

  // Huge difference in speed when using a transaction.
  // See: http://www.sqlite.org/cvstrac/wiki?p=PerformanceTuning
  db->execute ( "begin transaction" );

  const unsigned int total ( 1000 );
  for ( unsigned int i = 0; i < total; ++i )
  {
    std::string sql ( Usul::Strings::format ( "insert into ", table, " ( ", column0, ", ", column1, " ) values ( ", i, ", ", 10.0 * i, " )" ) );
    std::cout << "Insert statement " << i << " of " << total << ", SQL: " << sql << " ... ";
    db->execute ( sql );
    std::cout << "done" << std::endl;
  }

  db->execute ( "commit transaction" );

  ::_printResult ( db->execute ( Usul::Strings::format ( "select * from ", table ) ) );
  ::_printResult ( db->execute ( Usul::Strings::format ( "alter table ", table, " add ", column2, " double" ) ) );
  ::_printResult ( db->execute ( Usul::Strings::format ( "select * from ", table ) ) );
  ::_printResult ( db->execute ( Usul::Strings::format ( "select ", column0, ", ", column1, " from ", table ) ) );

  // Can we get two results before we access them?
  Result::RefPtr r1 ( db->execute ( Usul::Strings::format ( "select * from ", table ) ) );
  Result::RefPtr r2 ( db->execute ( Usul::Strings::format ( "select ", column0, ", ", column1, " from ", table ) ) );
  ::_printResult ( r1 );
  ::_printResult ( r2 );

  db = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::Functions::safeCall ( _test, "3729532207" );
  std::cout << "Press any key to exit" << std::endl;
  std::cin.get();
  return 0;
}
