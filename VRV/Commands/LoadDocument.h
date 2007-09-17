
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_COMMANDS_LOAD_DOCUMENT_H__
#define __VRV_COMMANDS_LOAD_DOCUMENT_H__

#include "Usul/Commands/Command.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Interfaces/IUnknown.h"

#include <vector>

namespace osg { class Node; }

namespace VRV {
namespace Commands {

class LoadDocument : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;
  typedef std::vector< std::string > Filenames;

  USUL_DECLARE_COMMAND ( LoadDocument );

  LoadDocument ( const Filenames& filenames, Usul::Interfaces::IUnknown *caller = 0x0 );

protected:
  virtual ~LoadDocument ();

  virtual void _execute ();

  /// Internal job class to load document.
  class LoadJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;

    LoadJob ( const Filenames& filenames, Usul::Interfaces::IUnknown *caller = 0x0 );

  protected:
    virtual ~LoadJob();

    virtual void              _started();

    void                      _loadModel ( const std::string& filename );

    // Post-process the model loading.
    void                      _postProcessModelLoad ( const std::string &filename, osg::Node *model );

  private:
    Filenames _filenames;
    Usul::Interfaces::IUnknown::QueryPtr _caller;
    Usul::Interfaces::IUnknown::QueryPtr _secondProgressBar;
  };

private:
  Filenames _filenames;
};

}
}

#endif // __VRV_COMMANDS_LOAD_DOCUMENT_H__
