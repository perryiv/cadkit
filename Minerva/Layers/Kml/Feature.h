
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_FEATURE_H__
#define __MINERVA_LAYERS_KML_FEATURE_H__

#include "Minerva/Layers/Kml/Object.h"


namespace Minerva {
namespace Layers {
namespace Kml {
      

class Feature : public Minerva::Layers::Kml::Object
{
public:
  typedef Minerva::Layers::Kml::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Feature );
  
  Feature();
  
protected:
  virtual ~Feature();
};


}
}
}

#endif // __MINERVA_LAYERS_KML_FEATURE_H__
