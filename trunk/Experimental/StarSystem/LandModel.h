
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

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec3f"

namespace StarSystem {

class LandModel : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( LandModel );

  LandModel () : BaseClass() { }

  // Get the size.
  virtual double      size () const = 0;
  
  // Get the elevation at given lat, lon.
  virtual double      elevation ( double lat, double lon ) const = 0;

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const = 0;

protected:
  virtual ~LandModel () {}
};

}

#endif // __STAR_SYSTEM_RASTER_LAND_MODEL_H__
