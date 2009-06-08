
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
#include "Minerva/Layers/OSM/Cache.h"

#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Data/Style.h"

#include "Usul/Jobs/Job.h"


namespace Minerva {
namespace Layers {
namespace OSM {

class OpenStreetMapXAPI : public Minerva::Core::Data::Container
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container BaseClass;
  typedef std::vector<Predicate> Predicates;
  typedef Usul::Jobs::Job::RefPtr JobPtr;
  typedef Minerva::Core::Data::Style Style;

  OpenStreetMapXAPI();

  /// Add a predicate request.
  void addRequest ( unsigned int level, const Predicate& predicate );

  /// Add a style for a predicate.  TODO: Add level parameter for selecting style.
  void addStyle ( const Predicate& predicate, Style::RefPtr );
  Style::RefPtr getStyle ( const Predicate& predicate ) const;
  
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
    Usul::Interfaces::IUnknown::RefPtr caller );

  /// Launch a job for the predicate.
  virtual JobPtr _launchJob ( 
    const Predicate& predicate, 
    const Extents& extents, 
    unsigned int level, 
    Usul::Jobs::Manager *manager, 
    Usul::Interfaces::IUnknown::RefPtr caller ) = 0;

  /// Get all the predicates to use at a given level.
  void _getAllPredicates ( unsigned int level, Predicates& predicates ) const;

  /// Initialize the cache.
  void _initializeCache ( const std::string& name );

  /// Get the cache.
  Cache::RefPtr _getCache() const;

private:
  
  typedef std::map<unsigned int, Predicates> RequestMap;
  typedef std::map<Predicate,Style::RefPtr> StyleMap;

  Cache::RefPtr _cache;
  std::string _url;
  RequestMap _requestMap;
  StyleMap _styleMap;

  SERIALIZE_XML_CLASS_NAME ( OpenStreetMapXAPI );
};



}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_X_API_H__
