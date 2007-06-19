
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_JOBS_LOAD_DIRECTORY_H__
#define __VRV_JOBS_LOAD_DIRECTORY_H__

#include "Usul/Jobs/Job.h"
#include "Usul/Interfaces/IUnknown.h"

namespace VRV {
namespace Jobs {

class LoadDirectory : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;

  LoadDirectory( const std::string& directory, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~LoadDirectory();

  virtual void              _started();
private:
  std::string _directory;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};

}
}

#endif // __VRV_JOBS_LOAD_DIRECTORY_H__
