
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV, Adam Kubach
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
#include "Usul/Adaptors/Random.h"

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

TestJob::TestJob ( unsigned long sleep ) : BaseClass(), 
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
//  Called when the job starts.
//
///////////////////////////////////////////////////////////////////////////////

void TestJob::_started()
{
  USUL_TRACE_SCOPE;

  // Show the progress bar if it's not already shown.  Hide when done.
  Usul::Interfaces::IProgressBar::ShowHide showHide ( this->progress() );

  const unsigned long id ( this->id() );

  std::ostringstream out;
  out << "  Started job " << SET_WIDTH << id 
      << " using thread " << SET_WIDTH << this->thread()->id()
      << " and system thread " << SET_WIDTH << this->thread()->systemId()
      << ", sleeping " << _sleep << '\n';
  TRACE_AND_PRINT ( out.str() );

  // Set the label.
  this->_setLabel ( out.str() );

  Usul::System::Sleep::milliseconds ( _sleep );

  // Time to sleep in each loop.
  Usul::Adaptors::Random < unsigned int > rand ( 100, 250 );
  unsigned int sleep ( rand() );

  // Loop and update progress.
  for ( unsigned int i = 0; i <= 100; ++i )
  {
    this->_updateProgress ( i, 100 );
    Usul::System::Sleep::milliseconds ( sleep );
  }
}
