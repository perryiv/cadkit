
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Helper functions to parse xml and create DataObjects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__

#include "Minerva/Layers/OSM/Common.h"

#include <string>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace OSM {

// Create a data object.
DataObject*          createForAllNodes ( const Nodes& nodes );
DataObject*          createForNode ( const Minerva::Layers::OSM::Node& node );
DataObject*          createForWay  ( const Minerva::Layers::OSM::Way&  way  );

// Parse the xml into ways and nodes.
void                 parseNodesAndWays ( const std::string& filename, Nodes& nodes, Ways& ways );
void                 parseNodesAndWays ( const XmlTree::Node& node, Nodes& nodes, Ways& ways );

Extents              parseExtents ( const XmlTree::Node& node );

}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_FUNCTIONS_H__
