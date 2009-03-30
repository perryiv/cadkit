
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job that uses the extended API (XAPI).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__

#include "Minerva/Layers/OSM/Cache.h"
#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/DownloadJob.h"

#include "Usul/Jobs/Job.h"
#include "Usul/Interfaces/ITileVectorJob.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class TileVectorJob : public Usul::Jobs::Job,
                      public Usul::Interfaces::ITileVectorJob
{
public:

  typedef Usul::Jobs::Job BaseClass;
  typedef std::vector<LineString::RefPtr> Lines;

  USUL_DECLARE_TYPE_ID ( TileVectorJob );
  USUL_DECLARE_REF_POINTERS ( TileVectorJob );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  TileVectorJob ( 
    Usul::Jobs::Manager* manager,
    Usul::Jobs::Manager* downloadManager, 
    Cache::RefPtr cache, 
    const std::string& url, 
    const Extents& extents, 
    const Predicate& predicate );

  Cache::RefPtr cache() const;
  std::string cacheKey() const;

  Extents extents() const;

protected:

  virtual ~TileVectorJob();

  /// Build the url for the given request type (i.e. "node", "way", "*" ).
  std::string _buildRequestUrl ( const std::string& requestType ) const;

  /// Job status functions.
  virtual void              _started();
  virtual void              _cancelled();

  /// Ask the sub-class to make it's request.
  virtual void              _makeRequest() = 0;

  /// Start a download of the given url.
  virtual void _startDownload ( const std::string& url );

  /// The downloading has finished.
  virtual void _downloadFinished ( const std::string& filename ) = 0;

  /// Add the unknown to our data.
  void _addData ( Usul::Interfaces::IUnknown* );

  /// Make enough room.
  void _reserveDataSize ( unsigned int size );

  /// Cancel the job (ITileVectorJob).
  virtual void                  cancelVectorJob();

  /// Get the container of data (ITileVectorJob).
  virtual Data                  getVectorData() const;

  /// Is the job done (ITileVectorJob)?
  virtual bool                  isVectorJobDone() const;

private:

  Usul::Jobs::Manager* _manager;
  Usul::Jobs::Manager* _downloadManager;
  DownloadJob::RefPtr _downloadJob;

  Cache::RefPtr _cache;
  std::string _url;
  std::string _filename;
  Extents _extents;
  Predicate _predicate;
  Data _data;
  bool _reAdd;

};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
