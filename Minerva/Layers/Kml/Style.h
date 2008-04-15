
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
#include "Minerva/Layers/Kml/PolyStyle.h"

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

	/// Set/get the polystyle.
	void           polystyle ( PolyStyle * );
	PolyStyle*     polystyle() const;

protected:
  
  virtual ~Style();
  
private:

	PolyStyle::RefPtr _polystyle;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_STYLE_H__
