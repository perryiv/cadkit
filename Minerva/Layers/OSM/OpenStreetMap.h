
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_H__

#include "Minerva/Core/Data/Container.h"

#include "Minerva/Layers/OSM/Node.h"
#include "Minerva/Layers/OSM/Way.h"

#include "Usul/Interfaces/IRead.h"

#include <vector>

namespace XmlTree { class Node; }

namespace Minerva { namespace Core { namespace Data { class DataObject; } } }

namespace Minerva {
namespace Layers {
namespace OSM {
  
class OpenStreetMap : public Minerva::Core::Data::Container,
                      public Usul::Interfaces::IRead
{
public:
  
  /// Typedefs.
  typedef Minerva::Core::Data::Container             BaseClass;
  typedef Minerva::Layers::OSM::Node::RefPtr         OSMNodePtr;
  typedef Minerva::Layers::OSM::Way::RefPtr          OSMWayPtr;
  typedef std::vector<OSMNodePtr>                    Nodes;
  typedef std::vector<OSMWayPtr>                     Ways;
  typedef Minerva::Core::Data::DataObject            DataObject;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( OpenStreetMap );
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  OpenStreetMap();
  
  // Read the file.
  virtual void                read ( const std::string &filename, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );
  
  // Deserialize.
  virtual void                deserialize ( const XmlTree::Node &node );
  
  /// Serialize.
  virtual void                serialize ( XmlTree::Node &parent ) const;
  
protected:

  virtual ~OpenStreetMap();

  // Create a data object.
  static DataObject*          _createForAllNodes ( const Nodes& nodes );
  static DataObject*          _createForNode ( const Minerva::Layers::OSM::Node& node );
  static DataObject*          _createForWay  ( const Minerva::Layers::OSM::Way&  way  );

  // Read.
  void                        _read ( const std::string &filename, Usul::Interfaces::IUnknown *caller, Usul::Interfaces::IUnknown *progress );

  // Parse the xml into ways and nodes.
  static void                 _parse ( const XmlTree::Node& node, Nodes& nodes, Ways& ways );
  
  // Set the bounds.
  void                        _setBounds ( const XmlTree::Node& node );
  
private:
  
  std::string _filename;
  
  SERIALIZE_XML_CLASS_NAME ( OpenStreetMap );
};

}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_H__
