
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

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Geometry"

#include <map>

namespace OsgTools {


class OSG_TOOLS_EXPORT ShapeFactory : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef std::pair < unsigned int, unsigned int > MeshSize;
  typedef std::pair < float, float > LatitudeRange;
  typedef LatitudeRange LongitudeRange;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ShapeFactory );

  // Constructor.
  ShapeFactory();

  // Create a sphere meshed in the latitude-longitude way. If one was 
  // already created with these inputs, then that same sphere is returned.
  osg::Geometry *         sphere ( float radius = 1.0f, 
                                   const MeshSize &size = MeshSize ( 20, 20 ),
                                   const LatitudeRange &latRange   = LatitudeRange  ( 90.0f, -90.0f ),
                                   const LongitudeRange &longRange = LongitudeRange (  0.0f, 360.0f ) );

  // Similar to above

protected:

  // use reference counting.
  virtual ~ShapeFactory();

private:

  // Typedefs.
  typedef osg::ref_ptr < osg::Geometry > SphereGeometry;
  typedef std::pair < float, MeshSize > SphereSize;
  typedef std::pair < LatitudeRange, LongitudeRange > SphereRange;
  typedef std::pair < SphereSize, SphereRange > SphereProperties;
  typedef std::map < SphereProperties, SphereGeometry > LatLongSpheres;

  LatLongSpheres _latLongSpheres;
};


}; // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_SHAPE_FACTORY_H_
