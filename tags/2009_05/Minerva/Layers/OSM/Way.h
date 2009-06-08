
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_WAY_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_WAY_H__

#include "Minerva/Layers/OSM/Object.h"
#include "Minerva/Layers/OSM/Node.h"

#include "Minerva/Core/Animate/Date.h"
#include "Minerva/Core/Extents.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec2d"

#include <vector>

namespace Minerva {
namespace Layers {
namespace OSM {


class MINERVA_OSM_EXPORT Way : public Minerva::Layers::OSM::Object
{
public:

  typedef Minerva::Layers::OSM::Object BaseClass;
  typedef std::vector<Node::RefPtr> Nodes;
  typedef Minerva::Core::Extents<osg::Vec2d>  Extents;
  
  USUL_DECLARE_REF_POINTERS ( Way );

  static Way*         create ( IdType id, const Date& timestamp, const Tags& tags, const Nodes& nodes );

  // Get the extents of this way.
  Extents             extents() const;

  // Get the number of nodes.
  unsigned int        numNodes() const;

  // Get the i'th node.
  Node*               node ( unsigned int i ) const;

  // Get the nodes.
  const Nodes&        nodes() const;

protected:
  
  virtual ~Way();

private:

  Way();
  Way ( IdType id, const Date& timestamp, const Tags& tags, const Nodes& nodes );

  Nodes _nodes;
};


}
}
}


#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_WAY_H__
