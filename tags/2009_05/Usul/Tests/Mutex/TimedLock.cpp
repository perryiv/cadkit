
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
//  Test program for Mutex::lock ( timeout ).
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Exceptions/TimedOut.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/System/Sleep.h"

//#define BOOST_TEST_MAIN
//#define BOOST_TEST_MODULE "Usul_Thread_Mutex_Try_Lock_Test"
#include "boost/test/unit_test.hpp"

#include "boost/thread/thread.hpp"

///////////////////////////////////////////////////////////////////////////////
//
//  Typedefs.
//
///////////////////////////////////////////////////////////////////////////////

typedef Usul::Threads::Mutex Mutex;
typedef Usul::Threads::Guard<Mutex> Guard;

BOOST_AUTO_TEST_SUITE( Usul_Mutex_TimedLock_Suite )


///////////////////////////////////////////////////////////////////////////////
//
//  Class to lock a mutex and then make a timed lock request.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  class TimedLockTestClass1
  {
  public:
    TimedLockTestClass1() : _mutex() {}

    void test1()
    {
      Guard guard ( _mutex );
      Usul::System::Sleep::milliseconds ( 10000 );
    }

    void test2()
    {
      bool timeout ( false );

      try
      {
        // This should throw an exception.
        _mutex.lock( 5000 );
      }
      catch ( const Usul::Exceptions::TimedOut::AcquireLock& )
      {
        std::cout << "AcquireLock exception caught.  This is expected. " << std::endl;
        timeout = true;
      }

      BOOST_CHECK ( timeout );
    }

  private:
    Mutex _mutex;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Test timed lock when mutex is locked by another thread.
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( TimedLockTest1 )
{
  BOOST_TEST_MESSAGE ( "Start Timed Lock Test 1." );

  Detail::TimedLockTestClass1 test;

  boost::thread thread1 ( Usul::Adaptors::memberFunction ( &test, &Detail::TimedLockTestClass1::test1 ) );
  boost::thread thread2 ( Usul::Adaptors::memberFunction ( &test, &Detail::TimedLockTestClass1::test2 ) );

  thread1.join();
  thread2.join();

  BOOST_TEST_MESSAGE ( "End Timed Lock Test 1." );
}

BOOST_AUTO_TEST_SUITE_END()
