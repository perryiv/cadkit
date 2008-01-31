
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

#include "Minerva/Core/Layers/VectorGroup.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IRead.h"

namespace Minerva { namespace Core { namespace DataObjects { class DataObject; } } }
namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace Kml {
  
  class NetworkLink;
  class Link;

class KmlLayer : public Minerva::Core::Layers::VectorGroup,
                 public Usul::Interfaces::IRead
{
public:
  /// Typedefs.
  typedef Minerva::Core::Layers::VectorGroup         BaseClass;
  typedef Minerva::Core::DataObjects::DataObject    DataObject;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( KmlLayer );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  KmlLayer();
  
  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  
  virtual void                deserialize( const XmlTree::Node &node );
protected:
  virtual ~KmlLayer();
  
  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Load a kml file.
  void                        _parseKml ( const std::string& filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );
  
  // Parse xml nodes.
  void                        _parseNode      ( const XmlTree::Node& node );
  void                        _parseStyle     ( const XmlTree::Node& node );
  void                        _parseFolder    ( const XmlTree::Node& node );
  DataObject*                 _parsePlacemark ( const XmlTree::Node& node );
  DataObject*                 _parseModel     ( const XmlTree::Node& node );
  DataObject::AltitudeMode    _parseAltitudeMode ( const XmlTree::Node& node );
  NetworkLink*                _parseNetworkLink ( const XmlTree::Node& node );
  Link*                       _parseLink        ( const XmlTree::Node& node );

  osg::Vec3                   _buildVec3      ( const XmlTree::Node& node );
private:
  std::string _filename;
  std::string _currentFilename;
  Minerva::Core::Layers::VectorGroup::RefPtr _currentLayer;
  
  SERIALIZE_XML_CLASS_NAME ( KmlLayer );
};
  
}
}
}


#endif // __MINERVA_LAYERS_KML_H__
