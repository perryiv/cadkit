
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_PROJECT_COORDINATES_H__
#define __USUL_PROJECT_COORDINATES_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

namespace Usul {
namespace Interfaces {


struct IProjectCoordinates : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IProjectCoordinates );

  /// Id for this interface.
  enum { IID = 1081536333u };

  /// Project to lat/lon with elevation using given spatial reference id.
  virtual void projectToSpherical ( const Usul::Math::Vec3d& orginal, unsigned int srid, Usul::Math::Vec3d& latLonPoint ) const = 0;

}; // struct IProjectCoordinates


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_PROJECT_COORDINATES_H__ */

