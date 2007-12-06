
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAND_MODEL_ELLIPSOID_H__
#define __STAR_SYSTEM_RASTER_LAND_MODEL_ELLIPSOID_H__

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector2.h"

#include "StarSystem/LandModel.h"

#include "osg/Vec3f"

class ossimEllipsoid;

namespace StarSystem {

class LandModelEllipsoid : public LandModel
{
public:
  typedef LandModel BaseClass;
  typedef Usul::Math::Vec2d Vec2d;

  USUL_DECLARE_REF_POINTERS ( LandModelEllipsoid );

  // Enumerations.
  enum
  {
    RADIUS_EQUATOR = 0,
    RADIUS_POLAR   = 1
  };

  LandModelEllipsoid ( const Vec2d &r );

  // Get the size.
  virtual double size () const;
  
  // Get the elevation at given lat, lon.
  virtual double      elevation ( double lat, double lon ) const;

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;

protected:
  virtual ~LandModelEllipsoid ();

private:
  ossimEllipsoid *_ellipsoid;
};

}

#endif // __STAR_SYSTEM_RASTER_LAND_MODEL_ELLIPSOID_H__
