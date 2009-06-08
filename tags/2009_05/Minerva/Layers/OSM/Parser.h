
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to parse xml.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_PARSER_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_PARSER_H__

#include "Minerva/Layers/OSM/Common.h"
#include "Minerva/Layers/OSM/LineString.h"

#include <string>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace OSM {

class Parser
{
public:

  typedef std::vector<LineString::RefPtr> Lines;

  // Parse the xml into ways and nodes.
  static void    parseNodesAndWays ( const std::string& filename, Nodes& nodes, Ways& ways );
  static void    parseNodesAndWays ( const XmlTree::Node& node, Nodes& nodes, Ways& ways );
  
  static void parseLines ( const std::string& filename, Lines& lines );

  static Extents parseExtents ( const XmlTree::Node& node );

private:

  typedef Minerva::Layers::OSM::Object OSMObject;
  typedef OSMObject::Tags Tags;

  static void _parseTags ( const XmlTree::Node& node, Tags& tags );
};


}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_PARSER_H__
