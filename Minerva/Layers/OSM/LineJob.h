
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Job to download lines.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_JOB_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_JOB_H__

#include "Minerva/Layers/OSM/TileVectorJob.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class LineJob : public TileVectorJob
{
public:
  typedef TileVectorJob BaseClass;

  LineJob ( 
    Usul::Jobs::Manager* manager, 
    Cache::RefPtr cache, 
    const std::string& url, 
    const Extents& extents, 
    unsigned int level,
    const Predicate& predicate,
    Usul::Interfaces::IUnknown::RefPtr caller );

protected:

  virtual ~LineJob();

  /// Make the request.  Check the cache first.
  virtual void _started();

  void _buildDataObjects ( const Lines& lines );

private:

  Usul::Interfaces::IUnknown::RefPtr _caller;
};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_LINE_JOB_H__
