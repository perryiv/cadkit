
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
#include "Minerva/Layers/Kml/Style.h"

#include "Minerva/Core/Layers/Container.h"
#include "Minerva/Core/Geometry/Geometry.h"

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
  class Feature;

class KmlLayer : public Minerva::Core::Layers::Container,
                 public Usul::Interfaces::IRead
                 
{
public:

  /// Typedefs.
  typedef Minerva::Core::Layers::Container           BaseClass;
  typedef Minerva::Core::DataObjects::DataObject     DataObject;
  typedef Minerva::Core::Geometry::Geometry          Geometry;
  typedef Usul::Math::Vec3d                          Vertex;
  typedef std::vector < Vertex >                     Vertices;
  typedef std::map<std::string,Style::RefPtr>        Styles;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( KmlLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  KmlLayer();
  KmlLayer( const XmlTree::Node& node, const std::string& filename, const std::string& directory, const Styles& styles );
  KmlLayer( Link* link, const Styles& styles );
  
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
  void                        _parsePlacemark    ( const XmlTree::Node& node );
  Geometry*                   _parseModel        ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parsePoint        ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parsePolygon      ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parseLineString   ( const XmlTree::Node& node, Style *style );
  Geometry*                   _parseLineRing     ( const XmlTree::Node& node, Style *style );
  void                        _parseMultiGeometry ( const XmlTree::Node& node, Style *style, DataObject& object );
  Geometry::AltitudeMode      _parseAltitudeMode ( const XmlTree::Node& node );
  void                        _parseCoordinates  ( const XmlTree::Node& node, Vertices& vertices );

	Style*                      _style ( const std::string& name );

  osg::Vec3                   _buildVec3         ( const XmlTree::Node& node );

private:
  
  enum STATUS_FLAGS
  {
    DOWNLOADING = 0x00000001,
    READING     = 0x00000002
  };
  
  std::string _filename;
  std::string _directory;
  Link::RefPtr _link;
  double _lastUpdate;
  unsigned int _flags;
	Styles _styles;
  
  SERIALIZE_XML_CLASS_NAME ( KmlLayer );
};
  
}
}
}


#endif // __MINERVA_LAYERS_KML_H__
