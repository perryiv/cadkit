
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenStreetMap layer that uses the extended API (XAPI).  See http://wiki.openstreetmap.org/index.php/Osmxapi
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__

#include "Minerva/Layers/OSM/Common.h"

#include "Minerva/Core/Data/Container.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class OpenStreetMapXAPI : public Minerva::Core::Data::Container
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container BaseClass;

  OpenStreetMapXAPI();
  
  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;

protected:

  virtual ~OpenStreetMapXAPI();

  /// Launch the jobs to fetch vector data.
  virtual Jobs                launchVectorJobs ( double minLon, 
                                                 double minLat, 
                                                 double maxLon, 
                                                 double maxLat, 
                                                 unsigned int level, 
                                                 Usul::Jobs::Manager *manager,
                                                 Usul::Interfaces::IUnknown *caller );

private:

  typedef std::vector<Predicate> Predicates;
  typedef std::map<unsigned int, Predicates> RequestMap;

  /// Get all the predicates to use at a given level.
  void _getAllPredicates ( unsigned int level, Predicates& predicates ) const;

  std::string _url;
  RequestMap _requestMap;

  SERIALIZE_XML_CLASS_NAME ( OpenStreetMapXAPI );
};

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__
