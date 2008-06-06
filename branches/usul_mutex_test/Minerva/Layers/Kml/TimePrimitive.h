
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_TIME_PRIMITIVE_H__
#define __MINERVA_LAYERS_KML_TIME_PRIMITIVE_H__

#include "Minerva/Layers/Kml/Object.h"

#include <string>

namespace Minerva { namespace Core { namespace Animate { class Date; } } }

namespace Minerva {
namespace Layers {
namespace Kml {
      
      
class TimePrimitive : public Minerva::Layers::Kml::Object
{
public:
  typedef Minerva::Layers::Kml::Object BaseClass;
  typedef Minerva::Core::Animate::Date Date;
  
  USUL_DECLARE_REF_POINTERS ( TimePrimitive );
  
  TimePrimitive();
  TimePrimitive ( const XmlTree::Node& node );
  
  static Date parse ( const std::string& value );

protected:
  
  virtual ~TimePrimitive();
};


}
}
}


#endif // __MINERVA_LAYERS_KML_TIME_PRIMITIVE_H__
