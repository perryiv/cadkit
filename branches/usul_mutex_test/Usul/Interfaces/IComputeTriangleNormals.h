
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_COMPUTE_TRIANGLE_NORMALS_H__
#define __USUL_INTERFACES_COMPUTE_TRIANGLE_NORMALS_H__

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Math/Vector3.h"

#include <vector>

namespace Usul {
namespace Interfaces {


struct IComputeTriangleNormals : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IComputeTriangleNormals );

  /// Id for this interface.
  enum { IID = 645043272u };

  typedef std::vector< Usul::Math::Vec3d >  Vertices;
  typedef std::vector< Usul::Math::Vec3d >  Normals;

  virtual void computeNormalsPerTriangle ( const Vertices& vertices, Normals& normals ) = 0;

};


}
}


#endif // __USUL_INTERFACES_COMPUTE_TRIANGLE_NORMALS_H__
