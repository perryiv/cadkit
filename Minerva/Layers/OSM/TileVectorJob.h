
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

  USUL_DECLARE_TYPE_ID ( TileVectorJob );
  USUL_DECLARE_REF_POINTERS ( TileVectorJob );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  TileVectorJob ( Cache::RefPtr cache, const std::string& url, const Extents& extents, const Predicate& predicate );

protected:

  virtual ~TileVectorJob();

  virtual void                  _started() = 0;

  /// Cancel the job (ITileVectorJob).
  virtual void                  cancelVectorJob();

  /// Get the container of data (ITileVectorJob).
  virtual Data                  getVectorData() const;

  /// Is the job done (ITileVectorJob)?
  virtual bool                  isVectorJobDone() const;

  Cache::RefPtr _cache;
  std::string _url;
  Extents _extents;
  Predicate _predicate;
  Data _data;

};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
