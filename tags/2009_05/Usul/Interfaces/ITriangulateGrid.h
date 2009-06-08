
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for creating triangle set from 2D grid.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_TRIANGULATE_GRID_H__
#define __USUL_INTERFACES_TRIANGULATE_GRID_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Containers/Array2D.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

namespace OsgTools { namespace Triangles { class TriangleSet; } }

namespace Usul {
namespace Interfaces {


struct ITriangulateGrid : public Usul::Interfaces::IUnknown
{
  // Typedefs.
  typedef double  ValueType;
  typedef Usul::Containers::Array2D<ValueType> Grid;
  typedef Usul::Math::Vec2d GridResolution;
  typedef OsgTools::Triangles::TriangleSet TriangleSet;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITriangulateGrid );

  /// Id for this interface.
  enum { IID = 3976379223u };

  // Build a triangle set from a grid of data.  GridResolution is the size of a grid cell in the x and y directions.
  virtual TriangleSet*  triangulateGrid ( const Grid &grid, 
                                          const GridResolution& resolution, 
                                          const Usul::Math::Vec3d& offset, 
                                          ValueType noDataValue,
                                          Usul::Interfaces::IUnknown* caller = 0x0 ) = 0;

}; // ITriangulateGrid


}
}


#endif // __USUL_INTERFACES_TRIANGULATE_GRID_H__
