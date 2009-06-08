
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
#include "Database/SQLite/Input.h"
#include "Database/SQLite/Result.h"
#include "Database/SQLite/Transaction.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Convert.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Vector3.h"
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
typedef CadKit::Database::SQLite::Transaction<Connection::RefPtr> Transaction;
typedef Usul::Convert::Type<double,std::string> DoubleToString;


///////////////////////////////////////////////////////////////////////////////
//
//  Check the boolean expression.
//
///////////////////////////////////////////////////////////////////////////////

void throwError ( const std::string &message )
{
  throw std::runtime_error ( message );
}

#define CHECK_EXPRESSION(expression)\
  if ( false == ( expression ) )\
  {\
    ::throwError ( Usul::Strings::format ( "Error 2543096340: Line: ", __LINE__, ", Expression: ", #expression ) );\
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Simple blob class.
//
///////////////////////////////////////////////////////////////////////////////

struct MyBlob1
{
  typedef std::vector<double> Numbers;
  MyBlob1 ( unsigned int size = 0 ) : _numbers ( size, 0 )
  {
    for ( unsigned int i = 0; i < _numbers.size(); ++i )
    {
      _numbers.at ( i ) = i;
    }
  }
  ~MyBlob1()
  {
  }
  template < class Itr > void assign ( Itr begin, Itr end )
  {
    _numbers.assign ( begin, end );
  }
  const unsigned char *getBytes() const
  {
    return reinterpret_cast < const unsigned char * > ( ( true == _numbers.empty() ) ? 0x0 : &_numbers[0] );
  }
  unsigned int getNumBytes() const
  {
    return ( _numbers.size() * sizeof ( Numbers::value_type ) );
  }
  MyBlob1 &operator = ( const MyBlob1 &right )
  {
    _numbers = right._numbers;
    return *this;
  }
  bool operator == ( const MyBlob1 &right ) const
  {
    return ( _numbers == right._numbers );
  }
private:
  MyBlob1 ( const MyBlob1 & );
  Numbers _numbers;
};


///////////////////////////////////////////////////////////////////////////////
//
//  More blob types.
//
///////////////////////////////////////////////////////////////////////////////

typedef std::vector<double> MyBlob2;
typedef std::vector<Usul::Math::Vec3d> MyBlob3;


///////////////////////////////////////////////////////////////////////////////
//
//  For simple blob types we can use this macro to define input operators.
//
///////////////////////////////////////////////////////////////////////////////

SQL_LITE_WRAP_DEFINE_VECTOR_INPUT_OPERATOR ( MyBlob3 );


///////////////////////////////////////////////////////////////////////////////
//
//  Custom input operator.
//
///////////////////////////////////////////////////////////////////////////////

CadKit::Database::SQLite::Result &operator >> ( CadKit::Database::SQLite::Result &result, MyBlob1 &t )
{
  typedef CadKit::Database::SQLite::Blob Blob;
  typedef MyBlob1::Numbers Numbers;
  typedef Numbers::value_type ValueType;
  typedef Numbers::size_type SizeType;

  USUL_ASSERT_SAME_TYPE ( SizeType, Blob::size_type );
  USUL_ASSERT_SAME_TYPE ( unsigned char, Blob::value_type );

  Blob blob;
  result >> blob;

  if ( 0 != ( blob.size() % sizeof ( ValueType ) ) )
  {
    throw std::runtime_error ( Usul::Strings::format
      ( "Error 4282221411: Blob size is not evenly divisible by ", sizeof ( ValueType ) ) );
  }

  const ValueType *temp ( reinterpret_cast < ValueType * > ( &blob[0] ) );
  const SizeType size ( blob.size() / sizeof ( ValueType ) );

  t.assign ( temp, temp + size );

  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make the binders for the blob types that are not already defined.
//
///////////////////////////////////////////////////////////////////////////////

SQL_LITE_WRAP_DEFINE_COMPLEX_OBJECT_BINDER ( MyBlob1 );
SQL_LITE_WRAP_DEFINE_VECTOR_BINDER ( MyBlob3 );


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

  Usul::Policies::TimeBased update ( 1000 );
  const unsigned int total ( 1000 );

  // Make blob data.
  const MyBlob1 blob1 ( 1000 );
  const MyBlob2 blob2 ( 1000 );
  const MyBlob3 blob3 ( 1000, MyBlob3::value_type ( 1, 2, 3 ) );

  // Local scope for writing to the database.
  {
    Connection::RefPtr db ( new Connection ( file ) );

    std::string sql ( Usul::Strings::format ( 
      "create table SpatialTable ( ", 
      "Indices integer primary key autoincrement, ", 
      "Geometry blob not null, ", 
      "TextData text not null, ", 
      "BinaryData1 blob not null, ", 
      "BinaryData2 blob not null, ", 
      "BinaryData3 blob not null )" ) );

    db->execute ( sql );

    // Local scope for transaction object.
    {
      // Huge difference in speed when using a transaction.
      // See: http://www.sqlite.org/cvstrac/wiki?p=PerformanceTuning
      Transaction transaction ( db );

      for ( unsigned int i = 0; i < total; ++i )
      {
        // Convert to string this way because it does not like scientific notation.
        const std::string x ( DoubleToString::convert ( i ) );
        const std::string y ( DoubleToString::convert ( i * 10.0 ) );

        // Make sql statement.
        // See http://www.sqlite.org/cvstrac/wiki?p=BlobExample
        sql = Usul::Strings::format ( 
          "insert into SpatialTable ",
          " ( Geometry, TextData, BinaryData1, BinaryData2, BinaryData3 ) values (",
          " GeomFromText ( 'POINT ( ", x, ' ', y, " )' ),", 
          " ?, ?, ?, ? )" );

        // Feedback.
        if ( true == update() )
        {
          std::cout << "Insert statement " << i << " of " << total << ", SQL: " << sql << std::endl;
        }

        // Execute the statement.
        db->execute ( sql, Usul::Strings::format ( "Blob ", i ), blob1, blob2, blob3 );
      }

      // Make it happen.
      transaction.commit();
    }
  }

  // Local scope for reading from the database.
  {
    Connection::RefPtr db ( new Connection ( file ) );

    Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );
    Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from SpatialTable where MBRContains ( BuildMBR ( 3,   30,  7,  70 ), Geometry )" ) ) );
    std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

    start = Usul::System::Clock::milliseconds();
    Helper::printResult ( db->execute ( Usul::Strings::format ( "select AsText ( Geometry ) from SpatialTable where MBRContains ( BuildMBR ( 30, 300, 70, 700 ), Geometry )" ) ) );
    std::cout << "Time for select statement = " << ( Usul::System::Clock::milliseconds() - start ) << " milliseconds" << std::endl;

    start = Usul::System::Clock::milliseconds();
    Result::RefPtr result ( db->execute ( Usul::Strings::format ( "select TextData, BinaryData1, BinaryData2, BinaryData3 from SpatialTable where MBRContains ( BuildMBR ( 20, 200, 60, 600 ), Geometry )" ) ) );
    Usul::Types::Uint64 duration ( Usul::System::Clock::milliseconds() - start );

    // Count the rows.
    unsigned int rows ( 0 );

    // Make sure data is accurate.
    if ( true == result.valid() )
    {
      CHECK_EXPRESSION ( 4 == result->numColumns() );
      while ( true == result->prepareNextRow() )
      {
        std::string text;
        MyBlob1 data1;
        MyBlob2 data2;
        MyBlob3 data3;

        *result >> text >> data1 >> data2 >> data3;
        std::cout << "Text = '" << text << "'" << std::endl;

        CHECK_EXPRESSION ( data1 == blob1 );
        CHECK_EXPRESSION ( data2 == blob2 );
        CHECK_EXPRESSION ( data3 == blob3 );

        ++rows;
      }
    }

    // Report the number of rows.
    std::cout << "Selected " << rows << " rows in " << duration << " milliseconds" << std::endl;
  }
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
