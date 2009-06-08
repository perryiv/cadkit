
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAND_MODEL_FLAT_H__
#define __MINERVA_CORE_RASTER_LAND_MODEL_FLAT_H__

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"

#include "Minerva/Core/TileEngine/LandModel.h"

#include "osg/Vec3d"

class ossimProjection;


namespace Minerva {
namespace Layers {
namespace Ossim {


class LandModelFlat : public Minerva::Core::TileEngine::LandModel
{
public:

  typedef Minerva::Core::TileEngine::LandModel BaseClass;
  typedef BaseClass::Extents Extents;
  typedef BaseClass::MeshSize MeshSize;

  USUL_DECLARE_REF_POINTERS ( LandModelFlat );

  LandModelFlat ( unsigned int pcscode = 0 );
  LandModelFlat ( const std::string& name );

  // Deserialize this instance.
  virtual void        deserialize ( const XmlTree::Node &node );

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, double& x, double& y, double& z ) const;
  virtual void        xyzToLatLonHeight ( double x, double y, double z, double& lat, double& lon, double& elevation ) const;

  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual Matrix      planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;
  
  // Return the appropriate mesh size.
  virtual MeshSize    meshSize ( const Extents &extents, const MeshSize &ms );

  // Serialize this instance.
  virtual void        serialize ( XmlTree::Node &parent ) const;

protected:

  virtual ~LandModelFlat();

private:

  ossimProjection *_projection;

  SERIALIZE_XML_CLASS_NAME ( LandModelFlat );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAND_MODEL_FLAT_H__
