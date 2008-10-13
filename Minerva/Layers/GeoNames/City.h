
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_GEO_NAMES_CITY_H__
#define __MINERVA_LAYERS_GEO_NAMES_CITY_H__

#include "Minerva/Layers/GeoNames/Export.h"

#include "Usul/Math/Vector2.h"

#include <string>

namespace XmlTree { class Node; }

namespace Minerva {
namespace Layers {
namespace GeoNames {
  
class MINERVA_GEO_NAMES_EXPORT City
{
public:
  
  City ( const XmlTree::Node& node );
  ~City();

  // Get the location.
  Usul::Math::Vec2d     location() const;

  // Get the name.
  std::string           name() const;

private:

  City();
  
  Usul::Math::Vec2d _location;
  std::string _name;
};
  
}
}
}


#endif // __MINERVA_LAYERS_GEO_NAMES_CITY_H__
