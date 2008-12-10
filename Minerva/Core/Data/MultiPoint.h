
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
//  Optimized class for drawing multiple points at once.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DATA_MULTI_POINT_H__
#define __MINERVA_CORE_DATA_MULTI_POINT_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/Geometry.h"

#include "Usul/Math/Vector3.h"

#include <vector>

namespace Minerva {
namespace Core {
namespace Data {
      
      
class MINERVA_EXPORT MultiPoint : public Geometry
{
public:
  typedef Geometry BaseClass;
  typedef Usul::Math::Vec3d Vertex;
  typedef std::vector<Vertex>  Vertices;
  typedef Usul::Math::Vec4f Color;

  USUL_DECLARE_TYPE_ID ( MultiPoint );
  USUL_DECLARE_QUERY_POINTERS ( MultiPoint );

  MultiPoint();

  /// Append a points.
  void                    append ( const Vertex & );
  void                    append ( const Vertices & );

  /// Set/get the color.
  void                    color ( const Color& );
  Color                   color() const;
          
  /// Is this geometry transparent?
  virtual bool            isSemiTransparent() const;
  
  /// Get/Set the size.
  float                   size() const;
  void                    size ( float );
  
  /// Get/Set the points.
  void                    points ( const Vertices & );
  Vertices                points() const;
  
protected:
  
  /// Use reference counting.
  virtual ~MultiPoint();
  
  /// Build the scene branch.
  virtual osg::Node*    _buildScene ( Usul::Interfaces::IUnknown * caller );
  
private:
  Vertices     _points;
  float        _size;
  Color        _color;
};

}
}
}


#endif // __MINERVA_CORE_DATA_MULTI_POINT_H__
