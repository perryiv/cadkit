
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
#include "Usul/Interfaces/GUI/IStatusBar.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class TileVectorJob : public Usul::Jobs::Job,
                      public Usul::Interfaces::ITileVectorJob,
                      public Usul::Interfaces::IStatusBar
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
    unsigned int level,
    const Predicate& predicate );

  Cache::RefPtr cache() const;

  Extents extents() const;

  unsigned int level() const;

protected:

  virtual ~TileVectorJob();

  /// Set the status.
  void _setStatus ( const std::string& status );

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
  virtual void                  takeVectorData ( Data& data );

  /// Is the job done (ITileVectorJob)?
  virtual bool                  isVectorJobDone() const;

  /// Set the "status bar" text (IStatusBar).
  virtual void setStatusBarText ( const std::string &text, bool force );

private:

  Usul::Jobs::Manager* _manager;

  Cache::RefPtr _cache;
  std::string _url;
  const Extents _extents;
  const unsigned int _level;
  Predicate _predicate;
  Data _data;
  bool _reAdd;

};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_TILE_VECTOR_JOB_H__
