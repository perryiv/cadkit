
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
#include "Minerva/Core/postGIS/Geometry.h"

class ossimGpt;

namespace Minerva {
namespace Core {
namespace postGIS {


class MINERVA_EXPORT Polygon : public Geometry
{
public:
  typedef Geometry BaseClass;

  Polygon ( DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );

  virtual osg::Node*               buildScene();
  virtual osg::Geometry*           buildGeometry();

protected:
  virtual ~Polygon();

  void                             _buildLatLongPoints();

private:
  typedef std::vector < ossimGpt > LatLongPoints;
  typedef std::vector < LatLongPoints > PointsList;

  PointsList _points;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

