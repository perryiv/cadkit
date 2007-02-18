
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating triangles from line loop.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGULATE_H__
#define __USUL_INTERFACES_TRIANGULATE_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include <vector>
#include <list>

namespace Usul {
namespace Interfaces {


struct ITriangulate : public Usul::Interfaces::IUnknown
{
  // Useful typedefs.
  typedef std::vector< Usul::Math::Vec2d >  Vertices;
  typedef std::vector< Usul::Math::Vec3d >  Vertices3D;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangulate );

  /// Id for this interface.
  enum { IID = 2740627666u };

  // Triangulate the loop.
  virtual void triangulate ( const Vertices &in, Vertices &out, Vertices3D &normalsOut ) = 0;

  virtual void triangulate3D ( const Vertices3D &in, Vertices3D &out, Vertices3D &normalsOut ) = 0;
};


}
}


#endif // __USUL_INTERFACES_TRIANGULATE_H__
