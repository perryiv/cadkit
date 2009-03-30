
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for roads.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OSM_XAPI_ROAD_LAYER_H__
#define __MINERVA_LAYERS_OSM_XAPI_ROAD_LAYER_H__

#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/OpenStreetMapXAPI.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class RoadLayer : public OpenStreetMapXAPI
{
public:
  
  /// Typedefs.
  typedef OpenStreetMapXAPI BaseClass;

  RoadLayer();

protected:

  virtual ~RoadLayer();

  /// Launch a job for the predicate.
  virtual JobPtr _launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Jobs::Manager *downloadManager, 
    Usul::Interfaces::IUnknown::RefPtr caller );

private:

  Cache::RefPtr _cache;

  SERIALIZE_XML_CLASS_NAME ( RoadLayer );
};

}
}
}


#endif // __MINERVA_LAYERS_OSM_XAPI_ROAD_LAYER_H__
