
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_H__
#define __MINERVA_LAYERS_KML_H__

#include "Minerva/Layers/Kml/Link.h"

#include "Minerva/Core/Layers/VectorGroup.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Math/Vector3.h"

#include <vector>

namespace Minerva { namespace Core { namespace DataObjects { class DataObject; } } }
namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {
  
  class NetworkLink;

class KmlLayer : public Minerva::Core::Layers::VectorGroup,
                 public Usul::Interfaces::IRead
                 
{
public:
  /// Typedefs.
  typedef Minerva::Core::Layers::VectorGroup         BaseClass;
  typedef Minerva::Core::DataObjects::DataObject     DataObject;
  typedef Usul::Math::Vec3d                          Vertex;
  typedef std::vector < Vertex >                     Vertices;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( KmlLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  KmlLayer();
  KmlLayer( const XmlTree::Node& node, const std::string& filename );
  KmlLayer( Link* link );
  
  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  void                        read ( Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  
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
  
protected:
  virtual ~KmlLayer();
  
  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Load a kml file.
  void                        _parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Update link.
  void                        _updateLink( Usul::Interfaces::IUnknown* caller = 0x0 );
  
  // Parse xml nodes.
  void                        _parseNode         ( const XmlTree::Node& node );
  void                        _parseStyle        ( const XmlTree::Node& node );
  void                        _parseFolder       ( const XmlTree::Node& node );
  DataObject*                 _parsePlacemark    ( const XmlTree::Node& node );
  DataObject*                 _parseModel        ( const XmlTree::Node& node );
  DataObject*                 _parsePoint        ( const XmlTree::Node& node );
  DataObject*                 _parsePolygon      ( const XmlTree::Node& node );
  DataObject*                 _parseLineString   ( const XmlTree::Node& node );
  DataObject*                 _parseLineRing     ( const XmlTree::Node& node );
  DataObject::AltitudeMode    _parseAltitudeMode ( const XmlTree::Node& node );
  NetworkLink*                _parseNetworkLink  ( const XmlTree::Node& node );
  Link*                       _parseLink         ( const XmlTree::Node& node );
  void                        _parseCoordinates  ( const XmlTree::Node& node, Vertices& vertices );

  osg::Vec3                   _buildVec3         ( const XmlTree::Node& node );
private:
  
  enum STATUS_FLAGS
  {
    DOWNLOADING = 0x00000001,
    READING     = 0x00000002
  };
  
  std::string _filename;
  Link::RefPtr _link;
  double _lastUpdate;
  unsigned int _flags;
  
  SERIALIZE_XML_CLASS_NAME ( KmlLayer );
};
  
}
}
}


#endif // __MINERVA_LAYERS_KML_H__
