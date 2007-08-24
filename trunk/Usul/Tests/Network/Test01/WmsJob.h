
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

#ifndef _USUL_TEST_WMS_JOB_CLASS_H_
#define _USUL_TEST_WMS_JOB_CLASS_H_

#include "Usul/Jobs/Job.h"
#include "Usul/Network/WMS.h"

namespace Usul { namespace Network { class WMS; } }


class WmsJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef Usul::Network::WMS WMS;
  typedef WMS::Options Options;

  WmsJob ( unsigned long id, const std::string &url, const std::string &file, const Options &options );

  virtual void              _started();

protected:

  virtual ~WmsJob();

private:

  WMS _wms;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Macro for printing and tracing at the same time.
//
///////////////////////////////////////////////////////////////////////////////

#define TRACE_AND_PRINT(exp)\
  USUL_TRACE_1 ( exp );\
  std::cout << exp << std::flush


#endif // _USUL_TEST_WMS_JOB_CLASS_H_
