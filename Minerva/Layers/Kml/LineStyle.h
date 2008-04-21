
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_LINE_STYLE_H__
#define __MINERVA_LAYERS_KML_LINE_STYLE_H__

#include "Minerva/Layers/Kml/ColorStyle.h"

namespace Minerva {
namespace Layers {
namespace Kml {
      

class LineStyle : public Minerva::Layers::Kml::ColorStyle
{
public:
  typedef Minerva::Layers::Kml::ColorStyle BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( LineStyle );
  
  LineStyle();
  LineStyle ( const XmlTree::Node& node );

	/// Set/get the width.
	void        width ( float w );
	float       width() const;

protected:
  
  virtual ~LineStyle();
  
private:

	float _width;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_LINE_STYLE_H__
