
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Test job.
//
///////////////////////////////////////////////////////////////////////////////

#include "TestJob.h"
#include "TestObject.h"

#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Threads/ThreadId.h"

#include <iostream>

USUL_IMPLEMENT_TYPE_ID ( TestJob );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TestJob::TestJob ( unsigned long sleep, TestObject *object ) : BaseClass(), 
  _sleep ( sleep ),
  _object ( object )
{
  Usul::Pointers::reference ( _object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TestJob::~TestJob()
{
  Usul::Pointers::unreference ( _object );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_started()
{
  if ( 0x0 == _object )
    return;

  const unsigned long id ( Usul::Threads::currentThreadId() );

  for ( unsigned int i = 0; i < 100; ++i )
  {
    // Temporarily gets a read-lock.
    const unsigned int data ( _object->getData() );

    std::cout << Usul::Strings::format ( "Thread: ", id, ", iteration: ", i, ", object's data: ", data, '\n' ) << std::flush;

    // Temporarily gets a write-lock.
    _object->setData ( ( i + 1 ) * data );

    // Tests combinations of locks.
    _object->exercise();

    //Usul::System::Sleep::milliseconds ( _sleep );
  }
}
