
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
//  Test program for job manager.
//
///////////////////////////////////////////////////////////////////////////////

#include "TestJob.h"

#include "Usul/Strings/Format.h"
#include "Usul/System/Sleep.h"
#include "Usul/Trace/Trace.h"

#include <iostream>
#include <iomanip>


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for setting width.
//
///////////////////////////////////////////////////////////////////////////////

#define SET_WIDTH std::setw ( 4 )


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TestJob::TestJob ( unsigned long sleep ) : BaseClass ( ), 
  _sleep ( sleep )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TestJob::~TestJob()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job is cancelled.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_cancelled()
{
  USUL_TRACE_SCOPE;

  std::ostringstream out;
  out << "Cancelled job " << SET_WIDTH << this->id() 
      << " using thread " << SET_WIDTH << this->thread()->id()
      << " and system thread " << SET_WIDTH << this->thread()->systemId() << '\n';
  TRACE_AND_PRINT ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job encounters an error.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_error()
{
  USUL_TRACE_SCOPE;

  std::ostringstream out;
  out << " Error in job " << SET_WIDTH << this->id() 
      << " using thread " << SET_WIDTH << this->thread()->id()
      << " and system thread " << SET_WIDTH << this->thread()->systemId() << '\n';
  TRACE_AND_PRINT ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job finishes normally.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_finished()
{
  USUL_TRACE_SCOPE;

  std::ostringstream out;
  out << " Finished job " << SET_WIDTH << this->id() 
      << " using thread " << SET_WIDTH << this->thread()->id()
      << " and system thread " << SET_WIDTH << this->thread()->systemId() << '\n';
  TRACE_AND_PRINT ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_started()
{
  USUL_TRACE_SCOPE;

  const unsigned long id ( this->id() );

  std::ostringstream out;
  out << "  Started job " << SET_WIDTH << id 
      << " using thread " << SET_WIDTH << this->thread()->id()
      << " and system thread " << SET_WIDTH << this->thread()->systemId()
      << ", sleeping " << _sleep << '\n';
  TRACE_AND_PRINT ( out.str() );

  //Usul::System::Sleep::milliseconds ( _sleep );

  unsigned long num ( 0 );

  for ( unsigned int i = 0; i < _sleep; ++i )
    num *= i;

  // Every 4th thread we cancel.
  if ( ( 0 != id ) && ( 0 == ( id % 4 ) ) )
  {
    this->cancel();
  }

  // Every 5th thread we generate an error.
  if ( ( 0 != id ) && ( 0 == ( id % 5 ) ) )
  {
    throw std::runtime_error ( "Error 2902393417: Generating error for testing" );
  }
}
