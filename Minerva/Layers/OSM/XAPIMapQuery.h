
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Make a map query that uses the extended API (XAPI).
//  See http://wiki.openstreetmap.org/index.php/Osmxapi
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OSM_XAPI_MAP_QUERY_H__
#define __MINERVA_LAYERS_OSM_XAPI_MAP_QUERY_H__

#include "Minerva/Layers/OSM/Cache.h"
#include "Minerva/Layers/OSM/Common.h"

namespace Minerva {
namespace Layers {
namespace OSM {


class XAPIMapQuery
{
public:
  XAPIMapQuery ( Cache::RefPtr cache, const std::string& url, const Predicate& pred, const Extents& extents );
  ~XAPIMapQuery();

  void makeNodesAndWaysQuery ( Nodes& nodes, Ways& ways );
  void makeNodesQuery ( Nodes& nodes );
  void makeWaysQuery ( Ways& ways );

private:
  
  Cache::RefPtr _cache;
  const std::string& _url;
  Predicate _predicate;
  Extents _extents;
};


}
}
}

#endif // __MINERVA_LAYERS_OSM_XAPI_MAP_QUERY_H__
