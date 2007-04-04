
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_LINE_GEOMETRY_H__
#define __MINERVA_POSTGIS_LINE_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/postGIS/Geometry.h"

#include "Usul/Interfaces/ILineData.h"

namespace Minerva {
namespace Core {
namespace postGIS {


class MINERVA_EXPORT Line : public Geometry,
                            public Usul::Interfaces::ILineData
{
public:
  typedef Geometry BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Line );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Line ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );

protected:
  ~Line();

  void                             _buildLatLongPoints();

  /// Usul::Interfaces::ILineData.
  virtual osg::Geometry*           buildLineData ();

private:
  typedef std::vector < ossimGpt > LatLongPoints;
  typedef std::vector < LatLongPoints > PointsList;

  PointsList _points;
};

}
}
}


#endif // __MINERVA_POSTGIS_LINE_GEOMETRY_H__
