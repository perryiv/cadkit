
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IOSSIM_PLANET_LAYER_H__
#define __USUL_INTERFACES_IOSSIM_PLANET_LAYER_H__

#include "Usul/Interfaces/IUnknown.h"

class ossimPlanetTextureLayer;

namespace Usul {
namespace Interfaces {


struct IOssimPlanetLayer : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IOssimPlanetLayer );

  /// Id for this interface.
  enum { IID = 1604465170u };

  virtual ossimPlanetTextureLayer *     ossimPlanetLayer() = 0;

}; // struct IOssimPlanetLayer


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IOSSIM_PLANET_LAYER_H__ */

