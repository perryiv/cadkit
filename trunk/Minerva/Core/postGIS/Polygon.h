
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
  typedef Line                             BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( Polygon );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Polygon ();

protected:
  virtual ~Polygon();

  void                             _buildLatLongPoints();

  /// Usul::Interfaces::IPolygonData
  virtual osg::Node*               buildPolygonData ();

private:

  Vertices _points;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

