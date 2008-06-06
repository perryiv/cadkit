
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

#include "SQLiteWrap/Database.h"
#include "SQLiteWrap/Results.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/File/Path.h"
#include "Usul/File/Remove.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/IO/Redirect.h"
#include "Usul/Threads/Mutex.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize mutex factory.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );


///////////////////////////////////////////////////////////////////////////////
//
//  Run a test.
//
///////////////////////////////////////////////////////////////////////////////

void _test ( SQLiteWrap::Callback *cb )
{
  const std::string file ( "test.sqlite" );
  Usul::File::remove ( file );
  SQLiteWrap::Database::ValidAccessRefPtr db ( new SQLiteWrap::Database ( file ) );
  db->callback ( cb );

  const std::string table ( "Array" );
  const std::string column0 ( "MyIndex" );
  const std::string column1 ( "Data" );
  const std::string column2 ( "Square" );

  db << "create table " << table << " ( " << column0 << " integer, " << column1 << " double )";
  db->execute();

  const unsigned int total ( 4 );
  for ( unsigned int i = 0; i < total; ++i )
  {
    std::ostringstream sql;
    db << "insert into " << table << " ( " << column0 << ", " << column1 << " ) values ( " << i << ", " << 10.0 * i << " )";
    db->execute();
  }

  db << "select * from " << table;
  db->execute();

  db << "alter table " << table << " add " << column2 << " double";
  db->execute();

  db << "select * from " << table;
  db->execute();

  db << "select " << column0 << " * " << column1 << " from " << table;
  db->execute();

  db = 0x0;
  Usul::File::remove ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _run()
{
  SQLiteWrap::Results::ValidRefPtr cb ( new SQLiteWrap::Results );
  _test ( cb.get() );
  cb->print ( std::cout );
  std::cout << std::flush;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  Usul::IO::Redirect redirect ( Usul::File::base ( argv[0] ) + ".txt", true, true );
  Usul::Functions::safeCall ( _run, "3729532207" );
  return 0;
}
