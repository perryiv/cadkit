
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_POLY_STYLE_H__
#define __MINERVA_LAYERS_KML_POLY_STYLE_H__

#include "Minerva/Core/Data/ColorStyle.h"

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT PolyStyle : public Minerva::Core::Data::ColorStyle
{
public:
  typedef Minerva::Core::Data::ColorStyle BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( PolyStyle );
  
  PolyStyle();

	/// Set/get the fill flag.
	void        fill ( bool b );
	bool        fill() const;

	/// Set/get the outline flag.
	void        outline ( bool b );
	bool        outline() const;

protected:
  
  virtual ~PolyStyle();
  
private:

	bool _fill;
	bool _outline;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_POLY_STYLE_H__
