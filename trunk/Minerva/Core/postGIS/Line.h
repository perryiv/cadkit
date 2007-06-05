
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
  typedef Geometry                         BaseClass;
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;

  USUL_DECLARE_QUERY_POINTERS ( Line );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Line ();
  Line ( Minerva::Core::DB::Connection *connection, const std::string &tableName, int id, int srid, const pqxx::result::field &F );

  /// Get/Set the line data.
  void                       line( const Vertices& );
  const Vertices&            line() const;

protected:
  ~Line();

  void                             _convertToLatLong ( const Vertices& vertices, Vertices& latLongPoints );
  void                             _buildLatLongPoints();

  /// Usul::Interfaces::ILineData.
  virtual osg::Geometry*           buildLineData ();
  virtual const Vertices&          lineData ();

private:
  typedef std::vector < ossimGpt > LatLongPoints;
  typedef std::vector < LatLongPoints > PointsList;

  Vertices   _line;
  Vertices   _latLongPoints;

  PointsList _points;
};

}
}
}


#endif // __MINERVA_POSTGIS_LINE_GEOMETRY_H__
