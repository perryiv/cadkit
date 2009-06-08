
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_GEO_RSS_H__
#define __MINERVA_LAYERS_GEO_RSS_H__

#include "Minerva/Layers/GeoRSS/Export.h"

#include "Minerva/Core/Data/Container.h"

#include "Usul/Interfaces/ITimerNotify.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "boost/date_time/posix_time/posix_time.hpp"

#include <vector>

namespace Minerva { namespace Core { namespace Data { class DataObject; } } }
namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace GeoRSS {  

class MINERVA_GEORSS_EXPORT GeoRSSLayer : public Minerva::Core::Data::Container,
                                          public Usul::Interfaces::ITimerNotify
                 
{
public:

  /// Typedefs.
  typedef Minerva::Core::Data::Container             BaseClass;
  typedef Minerva::Core::Data::DataObject            DataObject;
  typedef Usul::Math::Vec3d                          Vertex;
  typedef std::vector < Vertex >                     Vertices;
  typedef std::pair<TimerID,bool>                    TimerPair;
	typedef std::pair<std::string,std::string>         Filter;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( GeoRSSLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  GeoRSSLayer();

  /// Set/get the color.
  void                        color ( const Usul::Math::Vec4f& color );
  Usul::Math::Vec4f           color() const;

  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );

	// Launch a job to download feed.
  void                        downloadFeed();

  // Get/Set downloading flag.
  bool                        isDownloading() const;
  void                        downloading ( bool b );

	/// Set/get the filter.
	void                        filter ( const Filter& filter );
	Filter                      filter() const;

	/// Set/get the filtering enabled flag.
	void                        filteringEnabled ( bool b );
	bool                        filteringEnabled() const;

  /// Set/get the maximum age of items (The current time resolution is days).
  void                        maximumAge ( double );
  double                      maximumAge() const;

  /// Set/get the maximum number of items to show.
  void                        maximumItems ( unsigned int );
  unsigned int                maximumItems() const;

  // Get/Set reading flag.
  bool                        isReading() const;
  void                        reading ( bool b );

  /// Set/get the refresh rate (in seconds).
  void                        refreshRate ( double seconds );
  double                      refreshRate() const;

  /// Serialize
  virtual void                serialize ( XmlTree::Node &parent ) const;

  /// Set/get the url.
  void                        url ( const std::string& );
  std::string                 url() const;
  
  /// Set/get the use regular expression when filtering flag.
  void                        useRegEx ( bool );
  bool                        useRegEx() const;
  
protected:
  virtual ~GeoRSSLayer();

  // Add a timer callback.
  void                        _addTimer();

  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

  // Parse the item.
  void                        _parseItem ( const XmlTree::Node& );
  
  // Update link.
  void                        _updateLink ( Usul::Interfaces::IUnknown* caller = 0x0 );
  
  // Called when the timer fires.
  virtual void                timerNotify ( TimerID );
  
private:

  enum STATUS_FLAGS
  {
    DOWNLOADING       = 0x00000001,
    READING           = 0x00000002
  };
  
  boost::posix_time::ptime _lastDataUpdate;
  std::string _href;
  double _refreshInterval;
  unsigned int _flags;
  Usul::Math::Vec4f _color;
  TimerPair _timerInfo;
	Filter _filter;
  bool _filteringEnabled;
  bool _useRegEx;
  unsigned int _maximumItems;
  boost::posix_time::time_duration _maximumAge;
  
  SERIALIZE_XML_CLASS_NAME ( GeoRSSLayer );
};

}
}
}


#endif // __MINERVA_LAYERS_GEO_RSS_H__
