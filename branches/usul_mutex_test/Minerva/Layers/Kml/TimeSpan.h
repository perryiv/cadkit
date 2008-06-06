
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_TIME_SPAN_H__
#define __MINERVA_LAYERS_KML_TIME_SPAN_H__

#include "Minerva/Layers/Kml/TimePrimitive.h"
#include "Minerva/Core/Animate/Date.h"

namespace Minerva {
namespace Layers {
namespace Kml {


class TimeSpan : public Minerva::Layers::Kml::TimePrimitive
{
public:
  typedef Minerva::Layers::Kml::TimePrimitive BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( TimeSpan );
  
  TimeSpan();
  TimeSpan ( const XmlTree::Node& node );
  
  Date  begin() const;
  Date  end() const;
  
protected:
  
  virtual ~TimeSpan();
  
private:
  
  Date _begin;
  Date _end;
};


}
}
}


#endif // __MINERVA_LAYERS_KML_TIME_SPAN_H__
