
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OpenStreetMap layer that uses the extended API (XAPI).
//  See http://wiki.openstreetmap.org/index.php/Osmxapi
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__

#include "Minerva/Layers/OSM/Common.h"

#include "Minerva/Core/Data/Container.h"

#include "Usul/Jobs/Manager.h"

namespace Minerva {
namespace Layers {
namespace OSM {

class OpenStreetMapXAPI : public Minerva::Core::Data::Container
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container BaseClass;
  typedef std::vector<Predicate> Predicates;

  OpenStreetMapXAPI();

  /// Add a predicate request.
  void addRequest ( unsigned int level, const Predicate& predicate );
  
  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;

  /// Get the url.
  std::string url() const;

protected:

  virtual ~OpenStreetMapXAPI();

  /// Launch the jobs to fetch vector data.
  virtual Jobs launchVectorJobs ( 
    double minLon, 
    double minLat, 
    double maxLon, 
    double maxLat, 
    unsigned int level, 
    Usul::Jobs::Manager *manager,
    Usul::Interfaces::IUnknown::RefPtr caller ) = 0;

  /// Launch the jobs to fetch vector data.
  template<class JobType>
  Jobs _launchVectorJobs ( 
    Cache* cache,
    double minLon, 
    double minLat, 
    double maxLon, 
    double maxLat, 
    unsigned int level, 
    Usul::Jobs::Manager *manager,
    Usul::Interfaces::IUnknown::RefPtr caller );

  /// Get all the predicates to use at a given level.
  void _getAllPredicates ( unsigned int level, Predicates& predicates ) const;

private:
  
  typedef std::map<unsigned int, Predicates> RequestMap;

  std::string _url;
  RequestMap _requestMap;

  SERIALIZE_XML_CLASS_NAME ( OpenStreetMapXAPI );
};


///////////////////////////////////////////////////////////////////////////////
//
//  Launch the jobs to fetch vector data.
//
///////////////////////////////////////////////////////////////////////////////

template<class JobType>
OpenStreetMapXAPI::Jobs OpenStreetMapXAPI::_launchVectorJobs ( 
  Cache* cache,
  double minLon, 
  double minLat, 
  double maxLon, 
  double maxLat, 
  unsigned int level, 
  Usul::Jobs::Manager *manager,
  Usul::Interfaces::IUnknown::RefPtr caller )
{
  // Return now if there is no manager.
  if ( 0x0 == manager )
    return Jobs();

  // Get all the predicates.
  Predicates predicates;
  this->_getAllPredicates ( level, predicates );

  Jobs jobs;

  // Create a job for each predicate.
  for ( Predicates::const_iterator iter = predicates.begin(); iter != predicates.end(); ++iter )
  {
    typename JobType::RefPtr job ( new JobType ( cache, _url, Extents ( minLon, minLat, maxLon, maxLat ), *iter ) );
    jobs.push_back ( Usul::Interfaces::IUnknown::QueryPtr ( job ) );

    manager->addJob ( job.get() );
  }

  return jobs;
}


}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__
