
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_POINT_GEOMETRY_H__
#define __MINERVA_POSTGIS_POINT_GEOMETRY_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Data/Geometry.h"
#include "Minerva/Core/OSG/MatrixTransform.h"

#include "OsgTools/ShapeFactory.h"

#include "Usul/Math/Vector3.h"

namespace Minerva {
namespace Core {
namespace Data {


class MINERVA_EXPORT Point : public Geometry
{
public:
  typedef Geometry BaseClass;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec4f Color;
  typedef Minerva::Core::OSG::MatrixTransform MatrixTransform;

  USUL_DECLARE_QUERY_POINTERS ( Point );

  enum PrimitiveTypes
  {
    POINT = 1,
    SPHERE,
    CONE,
    DISK,
    CUBE,
    INVERTED_CONE,
    CYLINDER
  };
  
  Point();
  
  /// Get the shape factory.
  static OsgTools::ShapeFactory* shapeFactory();

  /// Set/get the color.
  void                    color ( const Color& );
  Color                   color() const;
  
  /// Elevation has changed within the given extents.
  virtual bool            elevationChangedNotify ( const Extents& extents, unsigned int level, ImagePtr elevationData, Unknown * caller );
  
  /// Is this geometry transparent?
  virtual bool            isSemiTransparent() const;
  
  /// Get/Set the size.
  float                   size() const;
  void                    size ( float );
  
  /// Get/Set the secondary size.
  float                   secondarySize() const;
  void                    secondarySize ( float );
  
  ///Get/Set the primitiveId.
  unsigned int            primitiveId() const;
  void                    primitiveId ( unsigned int );
  
  /// Get/Set quality
  void                    quality ( float value );
  float                   quality() const;
  
  /// Get/Set use auto transform flag.
  void                    autotransform ( bool b );
  bool                    autotransform() const;
  
  /// Get/Set the point.
  void                    point ( const Usul::Math::Vec3d & );
  const Vec3d             point() const;

  /// Get the point data as WGS 84.
  Vec3d                   pointData() const;
  
  /// Update.
  virtual void            updateNotify ( Usul::Interfaces::IUnknown *caller );
  
protected:
  
  /// Use reference counting.
  virtual ~Point();

  /// Build the scene branch.
  virtual osg::Node*    _buildScene ( Usul::Interfaces::IUnknown * caller );
  osg::Node*            _buildScene ( const Vec3d& point, double height, Usul::Interfaces::IUnknown * caller );
  
  osg::Node*            _buildGeometry( const osg::Vec3d& earthLocation, Usul::Interfaces::IUnknown* caller );
  
  osg::Node*            _buildPoint( const osg::Vec3d& earthLocation );
  osg::Node*            _buildSphere( const osg::Vec3d& earthLocation );
  osg::Node*            _buildCone( const osg::Vec3d& earthLocation, bool invert );
  osg::Node*            _buildDisk( const osg::Vec3d& earthLocation );
  osg::Node*            _buildCube( const osg::Vec3d& earthLocation );
  osg::Node*            _buildCylinder( const osg::Vec3d& earthLocation, Usul::Interfaces::IUnknown * caller );
  
private:
  Vec3d        _point;
  float        _size;
  float        _secondarySize;
  unsigned int _primitiveId;
  float        _quality;
  bool         _autotransform;
  Color        _color;
  MatrixTransform::RefPtr _transform;
  
  /// Shape Factory to share across all points.
  static OsgTools::ShapeFactory::Ptr _sf;
};

}
}
}


#endif // __MINERVA_POSTGIS_POINT_GEOMETRY_H__
