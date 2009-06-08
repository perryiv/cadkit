
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
//  Test program for the registry.
//
///////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
# ifndef NOMINMAX
#  define NOMINMAX
# endif
#endif

#include "XmlTree/RegistryIO.h"
#include "XmlTree/XercesLife.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/File/Contents.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Registry/Database.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Test one value.
//
///////////////////////////////////////////////////////////////////////////////

#define TEST_VALUE(the_type,the_key,the_value) \
{ \
  Usul::Registry::Database::instance() the_key = the_value; \
  USUL_ASSERT ( the_value == (Usul::Registry::Database::instance() the_key).get<the_type> ( the_value ) ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test one vector.
//
///////////////////////////////////////////////////////////////////////////////

#define TEST_VECTOR(the_type,the_key,the_vector) \
{ \
  Usul::Registry::Database::instance() the_key = the_vector; \
  USUL_ASSERT ( true == the_vector.equal ( (Usul::Registry::Database::instance() the_key).get<the_type> ( the_vector ) ) ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Populate the registry.
//
///////////////////////////////////////////////////////////////////////////////

void _populateRegistry()
{
  USUL_TRACE_SCOPE_STATIC;

  // This fauls up the file comparison with extra newlines.
#if 0
  // Put some value in the registry.
  TEST_VALUE ( std::string,  ["level_1_string"],  std::string ( "1" ) );
  TEST_VALUE ( float,        ["level_1_float"],   1.0f );
  TEST_VALUE ( double,       ["level_1_double"],  1.0  );
  TEST_VALUE ( unsigned int, ["level_1_uint"],    1    );
#endif

  // Second level.
  TEST_VALUE ( std::string,  ["level_1_string"]["level_2_string"], std::string ( "2" ) );
  TEST_VALUE ( float,        ["level_1_float" ]["level_2_float" ],  2.0f );
  TEST_VALUE ( double,       ["level_1_double"]["level_2_double"],  2.0 );
  TEST_VALUE ( unsigned int, ["level_1_uint"  ]["level_2_uint"  ],  2 );

  // Many levels and derived types.
  TEST_VECTOR ( Usul::Math::Vec3d,  ["level_one"]["level_two"]["level_three"]["level_four_1"], Usul::Math::Vec3d  ( 10.0,  20.0,  30.0  ) );
  TEST_VECTOR ( Usul::Math::Vec3f,  ["level_one"]["level_two"]["level_three"]["level_four_2"], Usul::Math::Vec3f  ( 10.0f, 20.0f, 30.0f ) );
  TEST_VECTOR ( Usul::Math::Vec3ui, ["level_one"]["level_two"]["level_three"]["level_four_3"], Usul::Math::Vec3ui ( 10,    20,    30    ) );
  TEST_VECTOR ( Usul::Math::Vec3i,  ["level_one"]["level_two"]["level_three"]["level_four_4"], Usul::Math::Vec3i  ( 10,    20,    30    ) );

  // Matrices
  TEST_VECTOR ( Usul::Math::Matrix44d, ["level_one"]["level_two"]["level_three"]["level_four_5"], Usul::Math::Matrix44d::translation ( 1.1, 2.2, 3.3 ) );
  TEST_VECTOR ( Usul::Math::Matrix44f, ["level_one"]["level_two"]["level_three"]["level_four_6"], Usul::Math::Matrix44d::translation ( 1.0, 2.0, 3.0 ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _runTest()
{
  USUL_TRACE_SCOPE_STATIC;

  // Scoped life.
  XmlTree::XercesLife xerces;

  // Registry file name.
  const std::string a ( "a.xml" );
  const std::string b ( "b.xml" );

  {
    // Scoped registry.
    Usul::Registry::Database::Life registry;

    // Populate the registry.
    ::_populateRegistry();

    // Write it to disk.
    XmlTree::RegistryIO::write ( a );
  }

  {
    // Scoped registry.
    Usul::Registry::Database::Life registry;

    // Read it from disk.
    XmlTree::RegistryIO::read ( a );

    // Write it to disk.
    XmlTree::RegistryIO::write ( b );

    // Write it to disk.
    XmlTree::RegistryIO::read ( b );
  }

  // Compare the files.
  std::string sa, sb;
  Usul::File::contents ( a, sa );
  Usul::File::contents ( b, sb );
  if ( sa != sb )
  {
    throw std::runtime_error ( "Error 1651017564: files are not equal" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Setup.
  Usul::Threads::Mutex::createFunction ( &Usul::Threads::newSingleThreadedMutexStub );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );
  std::ofstream trace ( "trace.csv" );
  Usul::Trace::Print::init ( &trace );

  // Call test function.
  Usul::Functions::safeCall ( _runTest, "9765950220" );

  return 0;
}
