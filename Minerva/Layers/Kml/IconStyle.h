
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_ICON_STYLE_H__
#define __MINERVA_LAYERS_KML_ICON_STYLE_H__

#include "Minerva/Layers/Kml/ColorStyle.h"

namespace Minerva {
namespace Layers {
namespace Kml {
      

class IconStyle : public Minerva::Layers::Kml::ColorStyle
{
public:
  typedef Minerva::Layers::Kml::ColorStyle BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( IconStyle );
  
  IconStyle();
  IconStyle ( const XmlTree::Node& node );

	

protected:
  
  virtual ~IconStyle();
  
private:

	
};


}
}
}

#endif // __MINERVA_LAYERS_KML_ICON_STYLE_H__
