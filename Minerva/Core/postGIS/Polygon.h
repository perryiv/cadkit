
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
#include "Minerva/Core/postGIS/Line.h"

#include "Usul/Interfaces/IPolygonData.h"
#include "Usul/Interfaces/ILineData.h"

class ossimGpt;

namespace Minerva {
namespace Core {
namespace postGIS {


class MINERVA_EXPORT Polygon : public Line,
                               public Usul::Interfaces::IPolygonData
{
public:
  typedef Line BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Polygon );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Polygon ( DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );

protected:
  virtual ~Polygon();

  void                             _buildLatLongPoints();

  /// Usul::Interfaces::IPolygonData
  virtual osg::Node*               buildPolygonData ();

private:
  typedef std::vector < ossimGpt > LatLongPoints;
  typedef std::vector < LatLongPoints > PointsList;

  PointsList _points;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

