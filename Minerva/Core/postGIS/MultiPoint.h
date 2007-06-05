
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_MULTI_POINT_H__
#define __MINERVA_POSTGIS_MULTI_POINT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/postGIS/Geometry.h"
#include "Minerva/Core/postGIS/Point.h"

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT MultiPoint : public Geometry
{
public:
  typedef Geometry BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( MultiPoint );

  MultiPoint();

  void addPoint( Point* point );
  void addGeometry ( Geometry *geometry );

protected:
  virtual ~MultiPoint();

private:
  typedef std::vector< Point::RefPtr > Points;

  Points _points;
};

}
}
}

#endif // __MINERVA_POSTGIS_MULTI_POINT_H__
