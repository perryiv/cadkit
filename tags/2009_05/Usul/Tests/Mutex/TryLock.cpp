
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test program for Mutex::trylock.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/System/Sleep.h"

//#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "Usul_Thread_Mutex_Try_Lock_Test"
#include "boost/test/unit_test.hpp"

#include "boost/thread/thread.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Mutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;

BOOST_AUTO_TEST_SUITE( Usul_Mutex_TryLock_Suite )


///////////////////////////////////////////////////////////////////////////////
//
//  Test trylock when mutex is locked by the current thread.
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( TryLockTest1 )
{
  BOOST_TEST_MESSAGE ( "Start Try Lock Test 1." );

  // Make the mutex.
  Mutex mutex;

  Guard guard ( mutex );

  BOOST_CHECK ( false == mutex.trylock() );

  BOOST_TEST_MESSAGE ( "End Try Lock Test 1." );
}

namespace Detail
{
  class Test2
  {
  public:
    Test2() : _mutex() {}

    void test1()
    {
      Guard guard ( _mutex );
      Usul::System::Sleep::milliseconds ( 10000 );
    }

    void test2()
    {
      BOOST_CHECK ( false == _mutex.trylock() );
    }

  private:
    Mutex _mutex;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test trylock when mutex is locked by another thread.
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( TryLockTest2 )
{
  BOOST_TEST_MESSAGE ( "Start Try Lock Test 2." );

  Detail::Test2 test;

  boost::thread thread1 ( Usul::Adaptors::memberFunction ( &test, &Detail::Test2::test1 ) );
  boost::thread thread2 ( Usul::Adaptors::memberFunction ( &test, &Detail::Test2::test2 ) );

  thread1.join();
  thread2.join();

  BOOST_TEST_MESSAGE ( "End Try Lock Test 2." );
}

BOOST_AUTO_TEST_SUITE_END()
