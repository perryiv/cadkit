
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_
#define _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_

#include "Export.h"

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

  // Constructor.
  ShapeFactory();

  // Create a sphere meshed in the latitude-longitude way. If one was 
  // already created with these inputs, then that same sphere is returned.
  osg::Geometry *         sphere ( float radius = 1.0f, 
                                   const MeshSize &size = MeshSize ( 20, 20 ),
                                   const LatitudeRange &latRange   = LatitudeRange  ( 89.9f, -89.9f ),
                                   const LongitudeRange &longRange = LongitudeRange (  0.0f, 360.0f ) );

  // Create a cube. If one was already created with these inputs, then that 
  // same sphere is returned.
  osg::Geometry *         cube ( const osg::Vec3 &size = osg::Vec3 ( 1.0f, 1.0f, 1.0f ) );

  // Clear the internal maps.
  void                    clear();

protected:

  // use reference counting.
  virtual ~ShapeFactory();

private:

  // Typedefs.
  typedef osg::ref_ptr < osg::Geometry > Geometry;
  typedef std::pair < float, MeshSize > SphereSize;
  typedef std::pair < LatitudeRange, LongitudeRange > SphereRange;
  typedef std::pair < SphereSize, SphereRange > SphereProperties;
  typedef std::map < SphereProperties, Geometry > LatLongSpheres;
  typedef std::map < osg::Vec3, Geometry > Cubes;

  LatLongSpheres _latLongSpheres;
  Cubes _cubes;
};


}; // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_
