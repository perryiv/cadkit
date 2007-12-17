
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


class STAR_SYSTEM_EXPORT LandModelFlat : public LandModel
{
public:

  typedef LandModel BaseClass;
  typedef BaseClass::Extents Extents;
  typedef BaseClass::MeshSize MeshSize;

  USUL_DECLARE_REF_POINTERS ( LandModelFlat );

  LandModelFlat ( unsigned int pcscode = 0 );
  LandModelFlat ( const std::string& name );

  // Deserialize this instance.
  virtual void        deserialize ( const XmlTree::Node &node );

  // Get the elevation at given lat, lon.
  virtual double      elevation ( double lat, double lon ) const;

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;

  // Return the appropriate mesh size.
  virtual MeshSize    meshSize ( const Extents &extents, const MeshSize &ms );

  // Get the size.
  virtual double      size() const;

  // Serialize this instance.
  virtual void        serialize ( XmlTree::Node &parent ) const;

protected:

  virtual ~LandModelFlat();

private:

  ossimProjection *_projection;

  SERIALIZE_XML_CLASS_NAME ( star_system_land_model_flat );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;};


}


#endif // __STAR_SYSTEM_RASTER_LAND_MODEL_FLAT_H__
