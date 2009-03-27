
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Specialized layer for city names.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OSM_XAPI_CITY_NAME_LAYER_H__
#define __MINERVA_LAYERS_OSM_XAPI_CITY_NAME_LAYER_H__

#include "Minerva/Layers/OSM/Cache.h"
#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/OpenStreetMapXAPI.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class CityNameLayer : public OpenStreetMapXAPI
{
public:
  
  /// Typedefs.
  typedef OpenStreetMapXAPI BaseClass;

  CityNameLayer();

protected:

  virtual ~CityNameLayer();

  /// Launch the jobs to fetch vector data.
  virtual Jobs                launchVectorJobs ( double minLon, 
                                                 double minLat, 
                                                 double maxLon, 
                                                 double maxLat, 
                                                 unsigned int level, 
                                                 Usul::Jobs::Manager *manager,
                                                 Usul::Interfaces::IUnknown::RefPtr caller );

private:

  Cache::RefPtr _cache;

  SERIALIZE_XML_CLASS_NAME ( CityNameLayer );
};

}
}
}


#endif // __MINERVA_LAYERS_OSM_XAPI_CITY_NAME_LAYER_H__
