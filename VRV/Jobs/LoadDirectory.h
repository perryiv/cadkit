
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
#include "Usul/Interfaces/GUI/IProgressBarFactory.h"

#include "VRV/Interfaces/IModelAdd.h"

namespace VRV {
namespace Jobs {

class LoadDirectory : public Usul::Jobs::Job,
  public Usul::Interfaces::IProgressBarFactory,
  public VRV::Interfaces::IModelAdd
{
public:
  typedef Usul::Jobs::Job BaseClass;

  LoadDirectory( const std::string& directory, Usul::Interfaces::IUnknown *caller = 0x0 );

  USUL_DECLARE_IUNKNOWN_MEMBERS;
protected:
  virtual ~LoadDirectory();

  virtual void              _started();

  virtual void              _loadModel ( const std::string& filename );

  // Add the new model.
  // This will re-direct to the caller.
  virtual void addModel ( osg::Node *model, const std::string& filename );

  // Create a progress bar.
  virtual Usul::Interfaces::IUnknown*  createProgressBar();

private:
  std::string _directory;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  Usul::Interfaces::IUnknown::QueryPtr _secondProgressBar;
};

}
}

#endif // __VRV_JOBS_LOAD_DIRECTORY_H__
