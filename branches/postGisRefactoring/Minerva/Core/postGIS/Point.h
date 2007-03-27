
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_POINT_GEOMETRY_H__
#define __MINERVA_POSTGIS_POINT_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/postGIS/Geometry.h"

namespace Minerva {
namespace Core {
namespace postGIS {


class MINERVA_EXPORT Point : public Geometry
{
public:
  typedef Geometry BaseClass;

  Point ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );

  virtual osg::Node*               buildScene();
  virtual osg::Geometry*           buildGeometry();

  virtual osg::Vec3                getCenter ( float xOffset, float yOffset, float zOffset );

protected:
  virtual ~Point();
};

}
}
}


#endif // __MINERVA_POSTGIS_POINT_GEOMETRY_H__
