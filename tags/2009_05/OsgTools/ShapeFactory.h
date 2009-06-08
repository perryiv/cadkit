
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_
#define _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_

#include "OsgTools/Export.h"

#include "osg/Geometry"

#include <map>

namespace OsgTools {


class OSG_TOOLS_EXPORT ShapeFactory : public osg::Referenced
{
public:

  // Typedefs.
  typedef osg::Referenced BaseClass;
  typedef osg::ref_ptr < ShapeFactory > Ptr;
  typedef std::pair < unsigned int, unsigned int > MeshSize;
  typedef std::pair < float, float > LatitudeRange;
  typedef LatitudeRange LongitudeRange;

  // Can use this class as a singleton.
  static ShapeFactory& instance();

  // Constructor.
  ShapeFactory();

  /// Create a sphere meshed in the latitude-longitude way. If one was 
  /// already created with these inputs, then that same sphere is returned.
  osg::Geometry *         sphere ( float radius = 1.0f, 
                                   const MeshSize &size = MeshSize ( 20, 20 ),
                                   const LatitudeRange &latRange   = LatitudeRange  ( 89.9f, -89.9f ),
                                   const LongitudeRange &longRange = LongitudeRange (  0.0f, 360.0f ) );

  /// Create a sphere meshed in the latitude-longitude way.  The vertices and normals are transform with the
  /// given matrix.  This function will not cache the results.
  osg::Geometry*          sphere ( const osg::Vec3 &trans,
                                   float radius = 1.0f, 
                                   const MeshSize &size = MeshSize ( 20, 20 ),
                                   const LatitudeRange &latRange   = LatitudeRange  ( 89.9f, -89.9f ),
                                   const LongitudeRange &longRange = LongitudeRange (  0.0f, 360.0f ) );

  /// Create a cone.  This will not cache the results.
  osg::Geometry *         cone ( const osg::Vec3& center, 
                                 float radius, 
                                 float height, 
                                 const osg::Quat& rotation = osg::Quat ( 0.0f, 0.0f, 0.0f, 1.0f ), 
                                 float tessellation = 1.0f );

  /// Create a cube. If one was already created with these inputs, then that 
  /// same cube is returned.
  osg::Geometry *         cube ( const osg::Vec3 &size = osg::Vec3 ( 1.0f, 1.0f, 1.0f ) );

  /// Create a cylinder. If one was already created with these inputs, then that 
  /// same cylinder is returned.  Can be built with or without tristrips.
  osg::Geometry *         cylinder ( float radius, 
                                     unsigned int sides, 
                                     const osg::Vec3& pointOne = osg::Vec3( 0.0, 0.0, 0.0 ),
                                     const osg::Vec3& pointTwo = osg::Vec3( 0.0, 1.0, 0.0 ),
                                     bool triStrip = true );

  /// Clear the internal maps.
  void                    clear();

protected:

  // use reference counting.
  virtual ~ShapeFactory();

  osg::Geometry *         _cylinderTriStrips ( float radius, unsigned int sides, const osg::Vec3& pointOne, const osg::Vec3& pointTwo );
  osg::Geometry *         _cylinderTriangles ( float radius, unsigned int sides, const osg::Vec3& pointOne, const osg::Vec3& pointTwo );

private:

  // Typedefs.
  typedef osg::ref_ptr < osg::Geometry > Geometry;
  typedef std::pair < float, MeshSize > SphereSize;
  typedef std::pair < LatitudeRange, LongitudeRange > SphereRange;
  typedef std::pair < SphereSize, SphereRange > SphereProperties;
  typedef std::pair < float, unsigned int > CylinderSize;
  typedef std::pair < osg::Vec3, osg::Vec3 > CylinderPoints;
  typedef std::pair < CylinderSize, CylinderPoints > CylinderProperties;
  typedef std::map < SphereProperties, Geometry > LatLongSpheres;
  typedef std::map < osg::Vec3, Geometry > Cubes;
  typedef std::map < CylinderProperties, Geometry > Cylinders;

  LatLongSpheres _latLongSpheres;
  Cubes _cubes;
  Cylinders _cylinders;
  Cylinders _cylindersTriangles;

  static ShapeFactory* _instance;
};


} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_
