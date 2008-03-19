
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__
#define __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Geometry/Line.h"

#include "Minerva/Interfaces/IPolygonData.h"


namespace Minerva {
namespace Core {
namespace Geometry {


class MINERVA_EXPORT Polygon : public Line,
                               public Minerva::Interfaces::IPolygonData
{
public:
  typedef Line                             BaseClass;
  typedef Minerva::Interfaces::IPolygonData::Vertices Vertices;

  USUL_DECLARE_QUERY_POINTERS ( Polygon );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Polygon ();
  
  void                                  outerBoundary ( const Vertices& );
  virtual const Vertices&               outerBoundary() const;

  void                                  addInnerBoundary ( const Vertices& );
  virtual const Boundaries&             innerBoundaries() const;

protected:
  virtual ~Polygon();

private:

  Boundaries _boundaries;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

