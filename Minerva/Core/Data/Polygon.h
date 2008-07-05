
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

#include "Usul/Math/Vector3.h"

#include <vector>

namespace osg { class Geometry; }

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

  USUL_DECLARE_QUERY_POINTERS ( Polygon );

  Polygon ();
  
  void                  outerBoundary ( const Vertices& );
  const Vertices&       outerBoundary() const;

  void                  addInnerBoundary ( const Vertices& );
  const Boundaries&     innerBoundaries() const;

  /// Set/get the border color flag.
  void                  borderColor ( const Color& color );
  Color                 borderColor() const;
  
  /// Set/get draw border flag.
  void                  showBorder( bool b );
  bool                  showBorder() const;
  
  /// Set/get draw interior flag.
  void                  showInterior( bool b );
  bool                  showInterior() const;
  
protected:
  virtual ~Polygon();
  
  /// Build the scene branch for the data object.
  virtual osg::Node*    _buildScene( Usul::Interfaces::IUnknown* caller );
  
  osg::Node*            _buildPolygons( Usul::Interfaces::IUnknown* caller );
  
  osg::Geometry*        _buildGeometry ( const Vertices& inVertices, Extents& e, Usul::Interfaces::IUnknown *caller );

private:

  Boundaries _boundaries;
  bool _showBorder;
  bool _showInterior;
  Color _borderColor;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

