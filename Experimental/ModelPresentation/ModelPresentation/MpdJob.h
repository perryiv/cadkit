
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_JOB_LOADING_H__
#define __MODELPRESENTATION_JOB_LOADING_H__

#include "MpdDefinitions.h"

#include "Usul/Commands/Command.h"
#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IDataSync.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Documents/Manager.h"


#include <string>

namespace osg { class Node; }

class MpdJob : public Usul::Jobs::Job
  {
    public:
      typedef Usul::Jobs::Job                           BaseClass;

      USUL_DECLARE_REF_POINTERS ( MpdJob );
      typedef Usul::Documents::Document::RefPtr DocumentPtr;
      typedef Usul::Interfaces::IDataSync IDataSync;
      typedef Usul::Interfaces::IUnknown IUnknown;
      typedef std::vector < std::string > Files;
      typedef Usul::Interfaces::IUnknown IUnknown;
      typedef Usul::Documents::Manager DocManager;
      typedef DocManager::DocumentInfo Info;
      typedef MpdDefinitions::MpdDynamicSets MpdDynamicSets;

      typedef std::vector< MpdDefinitions::MpdDynamicSet > MpdDynamicSets;

      MpdJob ( Usul::Documents::Document* document, 
               Usul::Interfaces::IUnknown *caller, 
               unsigned int index,
               const std::string &lockfile,
               const std::string &workingDir,
               MpdDefinitions::MpdDynamicSets &dynamicSets );
      
      unsigned int                        index(){ return _index; };

    protected:
      ~MpdJob();
      virtual void                        _started ();
      void                                _findFiles( unsigned int index, Usul::Interfaces::IUnknown *caller );
      void                                _loadNewDynamicFiles( std::string filename, unsigned int index, IUnknown *caller );
      void                                _parseNewFiles( Files files, unsigned int index, IUnknown *caller );
      osg::Node*                          _loadFile( const std::string& filename, IUnknown *caller, IUnknown *progress );

      void                                _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, IUnknown *progress );
        
    private:
      DocumentPtr                         _document;
      IUnknown::RefPtr                    _caller;
      unsigned int                        _index;
      std::string                         _lockfile;   

      MpdDynamicSets&                     _dynamicSets;
      std::string                         _workingDir;
  };


#endif // __MODELPRESENTATION_JOB_LOADING_H__
