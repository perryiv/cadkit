
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_ICON_STYLE_H__
#define __MINERVA_LAYERS_KML_ICON_STYLE_H__

#include "Minerva/Core/Data/ColorStyle.h"

namespace Minerva {
namespace Core {
namespace Data {
      

class MINERVA_EXPORT IconStyle : public Minerva::Core::Data::ColorStyle
{
public:
  typedef Minerva::Core::Data::ColorStyle BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( IconStyle );
  
  IconStyle();

  /// Set/get the scale.
  void           scale ( double );
	double         scale() const;

  /// Set/get the heading.
  void           heading ( double );
	double         heading() const;

  /// Set/get the href.
  void           href ( const std::string& );
	std::string    href() const;

protected:
  
  virtual ~IconStyle();
  
private:

	double _scale;
  double _heading;
  std::string _href;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_ICON_STYLE_H__
