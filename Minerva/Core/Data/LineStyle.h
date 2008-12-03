
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_LINE_STYLE_H__
#define __MINERVA_LAYERS_KML_LINE_STYLE_H__

#include "Minerva/Core/Data/ColorStyle.h"

namespace Minerva {
namespace Core {
namespace Data {
      

class MINERVA_EXPORT LineStyle : public Minerva::Core::Data::ColorStyle
{
public:
  typedef Minerva::Core::Data::ColorStyle BaseClass;

  USUL_DECLARE_TYPE_ID ( LineStyle );
  USUL_DECLARE_REF_POINTERS ( LineStyle );

  static LineStyle*  create ( const Color& color, float width );
  
  LineStyle();

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
