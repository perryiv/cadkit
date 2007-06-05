
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_MULTI_POLYGON_H__
#define __MINERVA_POSTGIS_MULTI_POLYGON_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/postGIS/Geometry.h"
#include "Minerva/Core/postGIS/Polygon.h"

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT MultiPolygon : public Geometry
{
  public:
  typedef Geometry BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( MultiPolygon );

  MultiPolygon();

  void         addPolygon  ( Polygon* polygon );
  void         addGeometry ( Geometry* geometry );

protected:
  virtual ~MultiPolygon();

private:
  typedef std::vector< Polygon::RefPtr > Polygons;

  Polygons _polygons;
};

}
}
}

#endif // __MINERVA_POSTGIS_MULTI_POLYGON_H__
