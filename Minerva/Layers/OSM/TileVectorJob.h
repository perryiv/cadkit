
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
#include "Minerva/Layers/OSM/XAPIMapQuery.h"

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
    Cache::RefPtr cache, 
    const std::string& url, 
    const Extents& extents, 
    const Predicate& predicate );

  Cache::RefPtr cache() const;

  Extents extents() const;

protected:

  virtual ~TileVectorJob();

  /// Job status functions.
  virtual void              _started() = 0;
  virtual void              _cancelled();

  XAPIMapQuery _makeQuery() const;

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

  Cache::RefPtr _cache;
  std::string _url;
  Extents _extents;
  Predicate _predicate;
  Data _data;
  bool _reAdd;

};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
