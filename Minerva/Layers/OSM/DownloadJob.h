
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to download file.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_DOWNLOAD_JOB_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_DOWNLOAD_JOB_H__

#include "Minerva/Layers/OSM/Export.h"

#include "Usul/Jobs/Job.h"

#include <string>

namespace Minerva {
namespace Layers {
namespace OSM {

class MINERVA_OSM_EXPORT DownloadJob : public Usul::Jobs::Job
{
public:

  typedef Usul::Jobs::Job BaseClass;

  USUL_DECLARE_TYPE_ID ( DownloadJob );
  USUL_DECLARE_REF_POINTERS ( DownloadJob );

  DownloadJob ( const std::string& url, const std::string& filename );

  std::string filename() const;

  virtual bool success() const;

protected:

  virtual ~DownloadJob();

  virtual void                  _started();

private:

  std::string _url;
  std::string _filename;
  bool _success;

};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
