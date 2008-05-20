
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_GEOMETRY_LINE_H__
#define __MINERVA_CORE_GEOMETRY_LINE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Geometry/Geometry.h"

#include "Usul/Math/Vector3.h"

namespace Minerva {
namespace Core {
namespace Geometry {


class MINERVA_EXPORT Line : public Geometry
{
public:
  typedef Geometry                         BaseClass;
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;

  USUL_DECLARE_QUERY_POINTERS ( Line );

  Line ();

  /// Get/Set the line data.
  void                  line( const Vertices& );
  const Vertices&       line() const;

  /// Set/get tessellate flag.
  void                  tessellate ( bool );
  bool                  tessellate() const;
  
  /// Get/Set the width
  float                 width() const;
  void                  width( float );
  
protected:
  virtual ~Line();

  void                  _convertToLatLong ( const Vertices& vertices, Vertices& latLongPoints );
  void                  _buildLatLongPoints();

  virtual osg::Node*    _buildScene( Usul::Interfaces::IUnknown* caller );
  osg::Node*            _buildScene( const osg::Vec4& color, Usul::Interfaces::IUnknown* caller );
  
  /// Get the line data as WGS 84.
  const Vertices&       _lineDataWgs84();

private:

  Vertices   _line;
  Vertices   _latLongPoints;
  float      _width;
  bool       _tessellate;
};

}
}
}


#endif // __MINERVA_CORE_GEOMETRY_LINE_H__
