
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
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
#include "Database/SQLite/Input.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Policies/Update.h"
#include "Usul/Scope/RemoveFile.h"
#include "Usul/Strings/Format.h"

#include <iostream>

#ifdef _MSC_VER
# pragma warning ( disable : 4822 ) // "local class member function does not have a body".
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs for this file.
//
///////////////////////////////////////////////////////////////////////////////

typedef CadKit::Database::SQLite::Connection Connection;
typedef CadKit::Database::SQLite::Result Result;
typedef CadKit::Database::SQLite::Blob Blob;
typedef Usul::Convert::Type<double,std::string> DoubleToString;


///////////////////////////////////////////////////////////////////////////////
//
//  Check the boolean expression.
//
///////////////////////////////////////////////////////////////////////////////

#define CHECK_EXPRESSION(expression)\
  if ( false == ( expression ) )\
  {\
    throw std::runtime_error ( Usul::Strings::format ( "Error 2543096340: Line: ", __LINE__, ", Expression: ", #expression ) );\
  }


///////////////////////////////////////////////////////////////////////////////
//
//  The blob data.
//
///////////////////////////////////////////////////////////////////////////////

struct MyBlob
{
  typedef std::vector<double> Numbers;
  MyBlob ( unsigned int size = 0 ) : _numbers ( size, 0 )
  {
    for ( unsigned int i = 0; i < _numbers.size(); ++i )
    {
      _numbers.at ( i ) = i;
    }
  }
  ~MyBlob()
  {
  }
  MyBlob &operator = ( const MyBlob &right )
  {
    _numbers = right._numbers;
    return *this;
  }
  bool operator == ( const MyBlob &right ) const
  {
    return ( _numbers == right._numbers );
  }
private:
  MyBlob ( const MyBlob & );
  Numbers _numbers;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Run a test.
//  See: http://www.gaia-gis.it/spatialite/spatialite-2.2_tutorial.html
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  const std::string file ( "test.sqlite" );
  Usul::Scope::RemoveFile removeFile ( file );
  Usul::File::remove ( file );
  Connection::ValidAccessRefPtr db ( new Connection ( file ) );

  const std::string table ( "SpatialTable" );
  const std::string column0 ( "Indices" );
  const std::string column1 ( "Geometry" );
  const std::string column2 ( "TextData" );
  const std::string column3 ( "BinaryData" );

  std::string sql ( Usul::Strings::format ( 
    "create table ", table, " ( ", 
    column0, " integer primary key autoincrement, ", 
    column1, " blob not null, ", 
    column2, " text not null, ", 
    column3, " blob not null )" ) );

  db->execute ( sql );

  Usul::Policies::TimeBased update ( 1000 );
  const unsigned int total ( 100000 );

  // Huge difference in speed when using a transaction.
  // See: http://www.sqlite.org/cvstrac/wiki?p=PerformanceTuning
  db->execute ( "begin transaction" );

  // Make blob data.
  const MyBlob blob ( 1000 );

  for ( unsigned int i = 0; i < total; ++i )
  {
    // Convert to string this way because it does not like scientific notation.
    const std::string x ( DoubleToString::convert ( i ) );
    const std::string y ( DoubleToString::convert ( i * 10.0 ) );

    // Make sql statement.
    // See http://www.sqlite.org/cvstrac/wiki?p=BlobExample
    sql = Usul::Strings::format ( 
      "insert into ", table, 
      " ( ", column1, ", ", column2, ", ", column3, " ) values (",
      " GeomFromText ( 'POINT ( ", x, ' ', y, " )' ),", 
      " ?, ? )" );

    // Feedback.
    if ( true == update() )
    {
      std::cout << "Insert statement " << i << " of " << total << ", SQL: " << sql << std::endl;
    }

    // Execute the statement.
    db->execute ( sql, Usul::Strings::format ( "Blob ", i ), blob );
  }

  db->execute ( "commit transaction" );

  Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );
  Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from ", table, " where MBRContains ( BuildMBR ( 3,   30,  7,  70 ), Geometry )" ) ) );
  std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

  start = Usul::System::Clock::milliseconds();
  Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from ", table, " where MBRContains ( BuildMBR ( 30, 300, 70, 700 ), Geometry )" ) ) );
  std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

  start = Usul::System::Clock::milliseconds();
  Result::RefPtr result ( db->execute ( Usul::Strings::format ( "select TextData, BinaryData from ", table, " where MBRContains ( BuildMBR ( 20, 200, 60, 600 ), Geometry )" ) ) );
  Usul::Types::Uint64 duration ( Usul::System::Clock::milliseconds() - start );

  // Count the rows.
  unsigned int rows ( 0 );

  // Make sure data is accurate.
  if ( true == result.valid() )
  {
    CHECK_EXPRESSION ( 2 == result->numColumns() );
    while ( true == result->prepareNextRow() )
    {
      std::string text;
      MyBlob data;
      *result >> text >> data;
      std::cout << "Text = '" << text << "'" << std::endl;
      CHECK_EXPRESSION ( data == blob );
      ++rows;
    }
  }

  // Report the number of rows.
  std::cout << "Selected " << rows << " rows in " << duration << " milliseconds" << std::endl;

  // Done with this.
  db = 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::Functions::safeCall ( _test, "3667491933" );
  std::cout << "Press any key to exit" << std::endl;
  std::cin.get();
  return 0;
}
