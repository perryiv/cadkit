
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAND_MODEL_H__
#define __STAR_SYSTEM_RASTER_LAND_MODEL_H__

#include "StarSystem/Export.h"
#include "StarSystem/Extents.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec3d"
#include "osg/Vec2d"

namespace osg { class Matrixd; }

namespace StarSystem {


class STAR_SYSTEM_EXPORT LandModel : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;

  USUL_DECLARE_REF_POINTERS ( LandModel );

  LandModel () : BaseClass(),
    SERIALIZE_XML_INITIALIZER_LIST
  {
  }

  // Get the size.
  virtual double      size() const = 0;
  
  // Get the elevation at given lat, lon.
  virtual double      elevation ( double lat, double lon ) const = 0;

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const = 0;
  virtual void        xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const = 0;

  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual osg::Matrixd planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const = 0;
  
  // Return the appropriate mesh size.
  virtual MeshSize    meshSize ( const Extents &extents, const MeshSize &ms ) = 0;

protected:

  virtual ~LandModel()
  {
  }

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( LandModel );
};


}


#endif // __STAR_SYSTEM_RASTER_LAND_MODEL_H__
