
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __POINT_DOCUMENT_JOB_LOADING_H__
#define __POINT_DOCUMENT_JOB_LOADING_H__

#include "Usul/Commands/Command.h"
#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Interfaces/IDataSync.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Types/Types.h"
#include "osg/Vec3"
#include "osg/Geometry"
#include <string>

namespace osg { class Node; }

class PointLoader : public Usul::Jobs::Job
  {
    public:
      typedef Usul::Jobs::Job                           BaseClass;

      USUL_DECLARE_REF_POINTERS ( PointLoader );
      typedef Usul::Interfaces::IUnknown IUnknown;
      typedef Usul::Documents::Manager DocManager;
      typedef DocManager::DocumentInfo Info;
      
      PointLoader ( const std::string &path, Usul::Types::Uint64 numPoints );

      osg::ref_ptr< osg::Vec3Array >      getData();
      
      bool                                foundNewData();
      void                                foundNewData( bool state );

    protected:
      virtual ~PointLoader();
      virtual void                        _started ();
      
        
    private:
      std::string                         _path;
      Usul::Types::Uint64                 _numPoints;
      bool                                _foundNewData;
      osg::ref_ptr< osg::Vec3Array >      _vertices;
  };
#endif // __POINT_DOCUMENT_JOB_LOADING_H__

