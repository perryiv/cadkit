
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

#include <vector>

namespace osg { class Node; }

namespace VRV {
namespace Jobs {

class LoadModel : public Usul::Jobs::Job
{
public:
  typedef Usul::Jobs::Job BaseClass;
  typedef std::vector< std::string > Filenames;

  LoadModel( const Filenames& filenames, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~LoadModel();

  virtual void              _started();
  virtual void              _finished();

  virtual void              _loadModel ( const std::string& filename );

  // Post-process the model loading.
  void                      _postProcessModelLoad ( const std::string &filename, osg::Node *model );

  // Update the progress.
  void                      _updateProgressCallback ( const std::string& filename, unsigned long bytes, unsigned long total );

private:
  Filenames _filenames;
  Usul::Interfaces::IUnknown::QueryPtr _caller;
  Usul::Interfaces::IUnknown::QueryPtr _secondProgressBar;
};

}
}

#endif // __VRV_JOBS_LOAD_MODEL_H__
