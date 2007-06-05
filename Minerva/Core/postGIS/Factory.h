
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_FACTORY_H__
#define __MINERVA_POSTGIS_FACTORY_H__

#include "Minerva/Core/Export.h"

namespace Minerva {
namespace Core {
namespace postGIS {

class Geometry;

class MINERVA_EXPORT Factory
{
public:
  static Factory& instance();

  Geometry*       createFromBinary ( const unsigned char *buffer ) const;
  Geometry*       createFromText   ( const char *buffer ) const;

private:
  Factory();
  ~Factory();

  static Factory *_instance;

};


}
}
}

#endif // __MINERVA_POSTGIS_FACTORY_H__
