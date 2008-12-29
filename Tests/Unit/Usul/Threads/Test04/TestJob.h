
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

#ifndef _USUL_TEST_JOBS_JOB_CLASS_H_
#define _USUL_TEST_JOBS_JOB_CLASS_H_

#include "Usul/Jobs/Job.h"

class TestObject;


class TestJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;

  USUL_DECLARE_REF_POINTERS ( TestJob );
  USUL_DECLARE_TYPE_ID ( TestJob );

  TestJob ( unsigned long sleep, TestObject * );

  virtual void              _started();

protected:

  virtual ~TestJob();

private:

  unsigned long _sleep;
  TestObject *_object;
};


#endif // _USUL_TEST_JOBS_JOB_CLASS_H_
