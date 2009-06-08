
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_OPEN_STREET_MAP_NODE_H__
#define __MINERVA_LAYERS_OPEN_STREET_MAP_NODE_H__

#include "Minerva/Layers/OSM/Object.h"

#include "Minerva/Core/Animate/Date.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Types/Types.h"

#include <map>

namespace Minerva {
namespace Layers {
namespace OSM {
  
  
class MINERVA_OSM_EXPORT Node : public Minerva::Layers::OSM::Object
{
public:
  
  typedef Minerva::Layers::OSM::Object BaseClass;
  typedef Usul::Math::Vec2d Location;
  
  USUL_DECLARE_REF_POINTERS ( Node );
  
  static Node*        create ( IdType id, const Location& location, const Date& timestamp, const Tags& tags );

  // Get the location.
  const Location&     location() const;

protected:
  
  virtual ~Node();
  
private:

  Node();
  Node ( IdType id, const Location& location, const Date& timestamp, const Tags& tags );

  Location _location;
};


}
}
}

#endif // __MINERVA_LAYERS_OPEN_STREET_MAP_NODE_H__
