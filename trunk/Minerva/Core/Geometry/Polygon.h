
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
#include "Minerva/Core/Geometry/Line.h"

#include "Minerva/Interfaces/IPolygonData.h"

namespace osg { class Geometry; }

namespace Minerva {
namespace Core {
namespace Geometry {


class MINERVA_EXPORT Polygon : public Line,
                               public Minerva::Interfaces::IPolygonData
{
public:
  typedef Line                             BaseClass;
  typedef Minerva::Interfaces::IPolygonData::Vertices Vertices;

  USUL_DECLARE_QUERY_POINTERS ( Polygon );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Polygon ();
  
  void                                  outerBoundary ( const Vertices& );
  virtual const Vertices&               outerBoundary() const;

  void                                  addInnerBoundary ( const Vertices& );
  virtual const Boundaries&             innerBoundaries() const;

  /// Set/get the border color flag.
  void                  borderColor ( const osg::Vec4& color );
  const osg::Vec4&      borderColor() const;
  
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
  osg::Vec4 _borderColor;
};

}
}
}


#endif // __MINERVA_POSTGIS_POLYGON_GEOMETRY_H__

