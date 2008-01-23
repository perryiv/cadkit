
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
#include "Minerva/Core/Geometry/Geometry.h"

#include "Usul/Interfaces/IPointData.h"

namespace Minerva {
namespace Core {
namespace Geometry {


class MINERVA_EXPORT Point : public Geometry,
                             public Usul::Interfaces::IPointData
{
public:
  typedef Geometry BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Point );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Point ();

  /// Get/Set the point.
  void                              point( const Usul::Math::Vec3d & );
  const Usul::Math::Vec3d           point() const;

protected:
  virtual ~Point();

  /// Usul::Interfaces::IPointData
  virtual Usul::Math::Vec3d         pointData ( );

  virtual osg::Vec3f                geometryCenter ( unsigned int& srid );
  virtual osg::Vec3f                geometryCenter ( const osg::Vec3f& offset, unsigned int& srid );

private:
  Usul::Math::Vec3d _point;
};

}
}
}


#endif // __MINERVA_POSTGIS_POINT_GEOMETRY_H__
