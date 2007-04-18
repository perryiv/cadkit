
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_PLANET_COORDINATES_H__
#define __USUL_PLANET_COORDINATES_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

namespace Usul {
namespace Interfaces {


struct IPlanetCoordinates : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPlanetCoordinates );

  /// Id for this interface.
  enum { IID = 2885858388u };

  virtual void convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const = 0;

}; // struct IPlanetCoordinates


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_PLANET_COORDINATES_H__ */

