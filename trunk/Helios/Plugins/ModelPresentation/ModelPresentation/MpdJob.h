
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

  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector < std::string > Files;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  
  typedef MpdDefinitions::MpdDynamicSets MpdDynamicSets;

  MpdJob ( Usul::Interfaces::IUnknown *caller,
           const std::string &workingDir,
           const std::string &searchDir, 
           const std::string &prefix, 
           const std::string &extension,
           Files current );

  MpdDefinitions::Groups              getData();
  MpdDefinitions::MpdDynamicSetHeader getHeader();
  void                                setHeader( MpdDefinitions::MpdDynamicSetHeader h );
  bool                                foundNewData();
  void                                foundNewData( bool state );

protected:
  virtual ~MpdJob();
  virtual void                        _started ();
  void                                _findFiles();
  void                                _loadNewDynamicFiles( std::string filename, IUnknown *caller, IUnknown *progress );
  void                                _parseNewFiles( Files files, IUnknown *caller, IUnknown *progress );
  osg::Node*                          _loadFile( const std::string& filename, IUnknown *caller, IUnknown *progress );

  void                                _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, IUnknown *progress );
    
private:
  MpdDefinitions::Groups              _root;
  std::string                         _workingDir;
  std::string                         _searchDir;
  std::string                         _prefix;
  std::string                         _extension;
  Files                               _currentFiles;
  bool                                _foundNewData;
};


#endif // __MODELPRESENTATION_JOB_LOADING_H__
