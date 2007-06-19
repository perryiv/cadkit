
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_JOBS_LOAD_MODEL_H__
#define __VRV_JOBS_LOAD_MODEL_H__

#include "Usul/Jobs/Job.h"
#include "Usul/Interfaces/IUnknown.h"

namespace osg { class Node; }

namespace VRV {
namespace Jobs {

class LoadModel : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;

  LoadModel( const std::string& filename, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~LoadModel();

  virtual void              _started();
  virtual void              _finished();

  // Post-process the model loading.
  void                      _postProcessModelLoad ( const std::string &filename, osg::Node *model );

  // Update the progress.
  void                      _updateProgress ( const std::string& filename, unsigned long bytes, unsigned long total );
private:
  std::string _filename;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
};

}
}

#endif // __VRV_JOBS_LOAD_MODEL_H__
