
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_STYLE_H__
#define __MINERVA_LAYERS_KML_STYLE_H__

#include "Minerva/Layers/Kml/Object.h"
#include "Minerva/Layers/Kml/LineStyle.h"
#include "Minerva/Layers/Kml/PolyStyle.h"
#include "Minerva/Layers/Kml/IconStyle.h"

namespace Minerva {
namespace Layers {
namespace Kml {
      

class Style : public Minerva::Layers::Kml::Object
{
public:
  typedef Minerva::Layers::Kml::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Style );
  
  Style();
  Style ( const XmlTree::Node& node );

  /// Set/get the iconstyle.
  void           iconstyle ( IconStyle * );
  IconStyle*     iconstyle() const;

  /// Set/get the linestyle.
	void           linestyle ( LineStyle * );
	LineStyle*     linestyle() const;

	/// Set/get the polystyle.
	void           polystyle ( PolyStyle * );
	PolyStyle*     polystyle() const;

protected:
  
  virtual ~Style();
  
private:

  IconStyle::RefPtr _iconstyle;
  LineStyle::RefPtr _linestyle;
	PolyStyle::RefPtr _polystyle;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_STYLE_H__
