
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_KML_OBJECT_H__
#define __MINERVA_LAYERS_KML_OBJECT_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>

namespace Minerva {
namespace Layers {
namespace Kml {
      
class Object : public Usul::Base::Referenced
{
public:
  typedef Usul::Base::Referenced BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( Object );
  
  Object();
  
protected:
  virtual ~Object();
  
private:
  std::string _id;
};


}
}
}

#endif // __MINERVA_LAYERS_KML_OBJECT_H__
