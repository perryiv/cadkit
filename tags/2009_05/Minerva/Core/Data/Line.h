
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
#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Core/Data/LineStyle.h"

#include "Usul/Math/Vector3.h"

namespace osg { class StateSet; }

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Line : public Geometry
{
public:
  typedef Geometry                         BaseClass;
  typedef Usul::Math::Vec3d                Vertex;
  typedef std::vector < Vertex >           Vertices;
  typedef ColorStyle::Color                Color;

  USUL_DECLARE_TYPE_ID ( Line );
  USUL_DECLARE_QUERY_POINTERS ( Line );

  Line();

  /// Call this when there is an intersection.
  virtual void          intersectNotify ( double x, double y, double z, double lon, double lat, double elev, Unknown::RefPtr tile, Unknown::RefPtr body, Unknown::RefPtr caller, Closest & );

  /// Is this geometry transparent?
  virtual bool          isSemiTransparent() const;

  /// Get/Set the line data.
  void                  line ( const Vertices& );
  Vertices              line() const;
  
  /// Get the line color.
  Color                 lineColor() const;
  
  /// Set/get the line style.
  void                  lineStyle ( LineStyle * );
  LineStyle*            lineStyle() const;

  /// Set/get tessellate flag.
  void                  tessellate ( bool );
  bool                  tessellate() const;
  
  /// Get the width.
  float                 width() const;

  /// Get/set flag to use a shader.
  bool                  useShader() const;
  void                  useShader ( bool b );
  
protected:

  virtual ~Line();

  virtual osg::Node*    _buildScene ( Usul::Interfaces::IUnknown* caller );
  osg::Node*            _buildScene ( const Color& color, Usul::Interfaces::IUnknown* caller );
    
private:
  
  // Set proper state.
  void                  _setState ( osg::StateSet*, const Color& color ) const;

  Vertices   _line;
  bool       _tessellate;
  LineStyle::RefPtr _lineStyle;
  bool _useShader;
};

}
}
}


#endif // __MINERVA_CORE_GEOMETRY_LINE_H__
