
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAND_MODEL_FLAT_H__
#define __STAR_SYSTEM_RASTER_LAND_MODEL_FLAT_H__

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include "StarSystem/LandModel.h"

#include "osg/Vec3f"

class ossimProjection;

namespace StarSystem {

class LandModelFlat : public LandModel
{
public:
  typedef LandModel BaseClass;

  USUL_DECLARE_REF_POINTERS ( LandModelFlat );

  LandModelFlat ( unsigned int pcscode );
  LandModelFlat ( const std::string& name );

  // Get the size.
  virtual double size () const;
  
  // Get the elevation at given lat, lon.
  virtual double      elevation ( double lat, double lon ) const;

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;

protected:
  virtual ~LandModelFlat ();

private:
  ossimProjection *_projection;
};

}

#endif // __STAR_SYSTEM_RASTER_LAND_MODEL_FLAT_H__
