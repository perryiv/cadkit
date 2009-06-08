
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_COMMON_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_COMMON_H__

#include "Minerva/Core/Extents.h"

#include "Minerva/Layers/OSM/Node.h"
#include "Minerva/Layers/OSM/Way.h"

#include "osg/Vec2d"

#include <vector>

namespace Minerva { namespace Core { namespace Data { class DataObject; } } }

namespace Minerva {
namespace Layers {
namespace OSM {

  /// Forward declarations.
  class Cache;

  /// Typedefs.
  typedef Minerva::Layers::OSM::Node::RefPtr  OSMNodePtr;
  typedef Minerva::Layers::OSM::Way::RefPtr   OSMWayPtr;
  typedef std::vector<OSMNodePtr>             Nodes;
  typedef std::vector<OSMWayPtr>              Ways;
  typedef Minerva::Core::Data::DataObject     DataObject;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;
  typedef std::pair<std::string,std::string>  Predicate;
}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_COMMON_H__
