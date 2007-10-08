
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

#include "Usul/CommandLine/Arguments.h"
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
  USUL_ASSERT ( the_value == (Usul::Registry::Database::instance() the_key).get<the_type>() ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test one vector.
//
///////////////////////////////////////////////////////////////////////////////

#define TEST_VECTOR(the_type,the_key,the_vector) \
{ \
  Usul::Registry::Database::instance() the_key = the_vector; \
  USUL_ASSERT ( true == the_vector.equal ( (Usul::Registry::Database::instance() the_key).get<the_type>() ) ); \
}


///////////////////////////////////////////////////////////////////////////////
//
//  Run the test.
//
///////////////////////////////////////////////////////////////////////////////

void _test()
{
  USUL_TRACE_SCOPE_STATIC;

  // Put some value in the registry.
  TEST_VALUE ( std::string,  ["level 1 string"],  std::string ( "1" ) );
  TEST_VALUE ( float,        ["level 1 float"],   1.0f );
  TEST_VALUE ( double,       ["level 1 double"],  1.0  );
  TEST_VALUE ( unsigned int, ["level 1 uint"],    1    );

  // Second level.
  TEST_VALUE ( std::string,  ["level 1 string"]["level 2 string"], std::string ( "2" ) );
  TEST_VALUE ( float,        ["level 1 float"]["level 2 float"],    2.0f );
  TEST_VALUE ( double,       ["level 1 double"]["level 2 double"],  2.0 );
  TEST_VALUE ( unsigned int, ["level 1 uint"]["level 2 uint"],      2 );

  // Many levels and derived types.
  TEST_VECTOR ( Usul::Math::Vec3d,  ["level one"]["level two"]["level three"]["level four 1"], Usul::Math::Vec3d  ( 10.0,  20.0,  30.0  ) );
  TEST_VECTOR ( Usul::Math::Vec3f,  ["level one"]["level two"]["level three"]["level four 2"], Usul::Math::Vec3f  ( 10.0f, 20.0f, 30.0f ) );
  TEST_VECTOR ( Usul::Math::Vec3ui, ["level one"]["level two"]["level three"]["level four 3"], Usul::Math::Vec3ui ( 10,    20,    30    ) );
  TEST_VECTOR ( Usul::Math::Vec3i,  ["level one"]["level two"]["level three"]["level four 4"], Usul::Math::Vec3i  ( 10,    20,    30    ) );

  // Matrices
  TEST_VECTOR ( Usul::Math::Matrix44d, ["level one"]["level two"]["level three"]["level four 5"], Usul::Math::Matrix44d::translation ( 1, 2, 3 ) );
  TEST_VECTOR ( Usul::Math::Matrix44f, ["level one"]["level two"]["level three"]["level four 6"], Usul::Math::Matrix44d::translation ( 1, 2, 3 ) );

  // Destroy the registry.
  Usul::Registry::Database::destroy();
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
  Usul::Functions::safeCall ( _test, "9765950220" );

  return 0;
}
