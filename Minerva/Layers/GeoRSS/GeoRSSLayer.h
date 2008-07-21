
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

#include "Minerva/Core/Layers/Container.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "boost/date_time/posix_time/posix_time.hpp"

#include <vector>

namespace Minerva { namespace Core { namespace Data { class DataObject; } } }
namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace GeoRSS {  

class MINERVA_GEORSS_EXPORT GeoRSSLayer : public Minerva::Core::Layers::Container,
                                          public Usul::Interfaces::IRead
                 
{
public:
  /// Typedefs.
  typedef Minerva::Core::Layers::Container           BaseClass;
  typedef Minerva::Core::Data::DataObject            DataObject;
  typedef Usul::Math::Vec3d                          Vertex;
  typedef std::vector < Vertex >                     Vertices;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( GeoRSSLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  GeoRSSLayer();

  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  // Update.
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Deserialize.
  virtual void                deserialize( const XmlTree::Node &node );

  // Get/Set downloading flag.
  bool                        isDownloading() const;
  void                        downloading( bool b );

  // Get/Set reading flag.
  bool                        isReading() const;
  void                        reading( bool b );
  
  /// Set/get the url.
  void                        url ( const std::string& );
  std::string                 url() const;
  
protected:
  virtual ~GeoRSSLayer();
  
  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Parse the item.
  void                        _parseItem ( const XmlTree::Node& );
  
  // Update link.
  void                        _updateLink( Usul::Interfaces::IUnknown* caller = 0x0 );
  
private:

  enum STATUS_FLAGS
  {
    DOWNLOADING = 0x00000001,
    READING     = 0x00000002
  };
  
  boost::posix_time::ptime _lastDataUpdate;
  std::string _filename;
  std::string _href;
  double _refreshInterval;
  double _lastUpdate;
  unsigned int _flags;
  Usul::Math::Vec4f _color;
  
  
  SERIALIZE_XML_CLASS_NAME ( GeoRSSLayer );
};

}
}
}


#endif // __MINERVA_LAYERS_GEO_RSS_H__
