
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_COLOR_STYLE_H__
#define __MINERVA_LAYERS_KML_COLOR_STYLE_H__

#include "Minerva/Core/Data/Object.h"

#include "Usul/Math/Vector4.h"

namespace Minerva {
namespace Layers {
namespace Kml {
      

class ColorStyle : public Minerva::Core::Data::Object
{
public:
  typedef Minerva::Core::Data::Object BaseClass;
	typedef Usul::Math::Vec4f Color;

  enum ColorMode
  {
    NORMAL,
    RANDOM
  };
  
  USUL_DECLARE_REF_POINTERS ( ColorStyle );
  
  ColorStyle();
  ColorStyle ( const XmlTree::Node& node );
  
	/// Set/get the color.
	void              color ( const Color& c );
	const Color&      color() const;

protected:
  
  virtual ~ColorStyle();
  
private:
  
	Color _color;
  ColorMode _mode;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_COLOR_STYLE_H__
