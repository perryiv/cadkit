
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to represent boundary data.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECTS_BOUNDARY_H__
#define __DATA_OBJECTS_BOUNDARY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/DataObjects/Line.h"
#include "Minerva/Interfaces/IPolygonData.h"

#include <map>

namespace Minerva {
namespace Core {
namespace DataObjects {


class MINERVA_EXPORT Polygon : public Line
{
public:
  typedef Minerva::Core::DataObjects::Line BaseClass;
  typedef Minerva::Interfaces::IPolygonData IPolygonData;
  typedef IPolygonData::Vertices            Vertices;
  typedef IPolygonData::Boundaries          Boundaries;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Polygon );

  Polygon();

  /// Accept the visitor.
  virtual void          accept ( Minerva::Core::Visitor& visitor );

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
  /// Use reference counting.
  virtual ~Polygon();

  /// Build the scene branch for the data object.
  virtual osg::Node*    _preBuildScene( Usul::Interfaces::IUnknown* caller = 0x0 );

  osg::Node*            _buildPolygons( Usul::Interfaces::IUnknown* caller );

  osg::Geometry*        _buildGeometry ( const Vertices& inVertices, Extents& e, Usul::Interfaces::IUnknown *caller );

private:

  bool _showBorder;
  bool _showInterior;
  osg::Vec4 _borderColor;
};

}
}
}

#endif // __DATA_OBJECTS_BOUNDARY_H__
