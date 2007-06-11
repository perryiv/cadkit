
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
//  Test job.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_TEST_JOBS_JOB_CLASS_H_
#define _USUL_TEST_JOBS_JOB_CLASS_H_

#include "Usul/Jobs/Job.h"


class TestJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;

  TestJob ( unsigned long sleep );

  virtual void              _cancelled();

  virtual void              _error();

  virtual void              _finished();

  virtual void              _started();

protected:

  virtual ~TestJob();

private:

  unsigned long _sleep;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for printing and tracing at the same time.
//
///////////////////////////////////////////////////////////////////////////////

#define TRACE_AND_PRINT(exp)\
  USUL_TRACE_1 ( exp );\
  std::cout << exp << std::flush


#endif // _USUL_TEST_JOBS_JOB_CLASS_H_
