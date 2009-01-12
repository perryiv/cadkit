
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
//  Test program for SQLiteWrap using spatial extensions for sqlite.
//
///////////////////////////////////////////////////////////////////////////////

#include "Common/Functions.h"

#include "Database/SQLite/Connection.h"
#include "Database/SQLite/Result.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Policies/Update.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Run a test.
//  See: http://www.gaia-gis.it/spatialite/spatialite-2.2_tutorial.html
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  typedef CadKit::Database::SQLite::Connection Connection;
  typedef CadKit::Database::SQLite::Result Result;
  typedef Usul::Convert::Type<double,std::string> DoubleToString;

  const std::string file ( "test.sqlite" );
  Usul::Scope::RemoveFile removeFile ( file );
  Usul::File::remove ( file );
  Connection::ValidAccessRefPtr db ( new Connection ( file ) );

  const std::string table ( "SpatialTable" );
  const std::string column0 ( "Indices" );
  const std::string column1 ( "Geometry" );

  std::string sql ( Usul::Strings::format ( "create table ", table, " ( ", column0, " integer primary key autoincrement, ", column1, " blob not null )" ) );
  db->execute ( sql );

  Usul::Policies::TimeBased update ( 1000 );
  const unsigned int total ( 100000 );

  // Huge difference in speed when using a transaction.
  // See: http://www.sqlite.org/cvstrac/wiki?p=PerformanceTuning
  db->execute ( "begin transaction" );

  for ( unsigned int i = 0; i < total; ++i )
  {
    // Convert to string this way because it does not like scientific notation.
    const std::string x ( DoubleToString::convert ( i ) );
    const std::string y ( DoubleToString::convert ( i * 10.0 ) );

    // Make sql statement.
    sql = Usul::Strings::format ( "insert into ", table, " ( ", column1, " ) values ( GeomFromText ( 'POINT ( ", x, ' ', y, " )' ) )" );

    // Feedback.
    if ( true == update() )
    {
      std::cout << "Insert statement " << i << " of " << total << ", SQL: " << sql << std::endl;
    }

    // Execute the statement.
    db->execute ( sql );
  }

  db->execute ( "commit transaction" );

  Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );
  Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from ", table, " where MBRContains ( BuildMBR ( 3,   30,  7,  70 ), Geometry )" ) ) );
  std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

  start = Usul::System::Clock::milliseconds();
  Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from ", table, " where MBRContains ( BuildMBR ( 30, 300, 70, 700 ), Geometry )" ) ) );
  std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

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
