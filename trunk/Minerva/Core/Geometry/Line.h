
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

#include "Minerva/Interfaces/ILineData.h"

namespace Minerva {
namespace Core {
namespace Geometry {


class MINERVA_EXPORT Line : public Geometry,
                            public Minerva::Interfaces::ILineData
{
public:
  typedef Geometry                         BaseClass;
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;

  USUL_DECLARE_QUERY_POINTERS ( Line );
  USUL_DECLARE_IUNKNOWN_MEMBERS;

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
  
  /// Usul::Interfaces::ILineData.
  virtual const Vertices&          lineData();

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
