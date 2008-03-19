
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_IPOLYGON_DATA_H__
#define __MINERVA_INTERFACES_IPOLYGON_DATA_H__

#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector3.h"

#include <vector>

namespace Minerva {
namespace Interfaces {


struct IPolygonData : public Usul::Interfaces::IUnknown
{
  /// Typedefs.
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;
  typedef std::vector<Vertices>            Boundaries;
  
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IPolygonData );

  /// Id for this interface.
  enum { IID = 2101968926u };

  virtual const Vertices&               outerBoundary() const = 0;
  virtual const Boundaries&             innerBoundaries() const = 0;
  
}; // struct IPolygonData


} // end namespace Interfaces
} // end namespace Minerva


#endif /* __MINERVA_INTERFACES_IPOLYGON_DATA_H__ */
