
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
#include "Minerva/Core/Data/Line.h"
#include "Minerva/Core/Data/PolyStyle.h"

#include "Usul/Math/Vector3.h"

#include <vector>

namespace osg { class Geometry; }
namespace Usul { namespace Interfaces { struct IPlanetCoordinates; struct IElevationDatabase; } }


namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Polygon : public Line
{
public:
  typedef Line                             BaseClass;
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;
  typedef std::vector<Vertices>            Boundaries;
  typedef Usul::Interfaces::IPlanetCoordinates IPlanetCoordinates;
  typedef Usul::Interfaces::IElevationDatabase IElevationDatabase;

  USUL_DECLARE_QUERY_POINTERS ( Polygon );

  Polygon();

  void                  outerBoundary ( const Vertices& );
  Vertices              outerBoundary() const;

  void                  addInnerBoundary ( const Vertices& );
  const Boundaries&     innerBoundaries() const;

  /// Get the border color flag.
  Color                 borderColor() const;

  /// Get the fill color flag.
  Color                 fillColor() const;

  /// Is this geometry transparent?
  virtual bool          isSemiTransparent() const;

  /// Set/get the PolyStyle.
  void                  polyStyle ( PolyStyle * );
  PolyStyle*            polyStyle() const;

  /// Get draw border flag.
  bool                  showBorder() const;

  /// Get draw interior flag.
  bool                  showInterior() const;

protected:
  virtual ~Polygon();
  
  /// Build the scene branch for the data object.
  virtual osg::Node*    _buildScene ( Usul::Interfaces::IUnknown* caller );
  
  osg::Node*            _buildPolygons( Usul::Interfaces::IUnknown* caller );
  
  osg::Node*            _buildGeometry ( const Vertices& inVertices, Extents& e, Usul::Interfaces::IUnknown *caller );
  osg::Node*            _extrudeToGround ( const Vertices& inVertices, Usul::Interfaces::IUnknown *caller );

  Vertex                _convertToPlanetCoordinates ( const Polygon::Vertex& v, IPlanetCoordinates* planet, IElevationDatabase* elevation ) const;

private:

  Boundaries _boundaries;
  PolyStyle::RefPtr _polyStyle;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

