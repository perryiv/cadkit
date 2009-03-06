
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAND_MODEL_ELLIPSOID_H__
#define __MINERVA_CORE_RASTER_LAND_MODEL_ELLIPSOID_H__

#include "Usul/Base/Object.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector2.h"

#include "Minerva/Core/TileEngine/LandModel.h"

namespace osg { class EllipsoidModel; }

namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT LandModelEllipsoid : public LandModel
{
public:

  typedef LandModel BaseClass;
  typedef BaseClass::Extents Extents;
  typedef BaseClass::MeshSize MeshSize;
  typedef Usul::Math::Vec2d Vec2d;

  USUL_DECLARE_REF_POINTERS ( LandModelEllipsoid );

  // Enumerations.
  enum
  {
    RADIUS_EQUATOR = 0,
    RADIUS_POLAR   = 1
  };

  LandModelEllipsoid ( const Vec2d &r = Vec2d ( 1, 1 ) );

  // Deserialize this instance.
  virtual void        deserialize ( const XmlTree::Node &node );

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, double& x, double& y, double& z ) const;
  virtual void        xyzToLatLonHeight ( double x, double y, double z, double& lat, double& lon, double& elevation ) const;
  
  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual Matrix      planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;

  // Return the appropriate mesh size.
  virtual MeshSize    meshSize ( const Extents &extents, const MeshSize &ms );

  // Return the radii.
  double              radiusEquator() const;
  double              radiusPolar() const;

  // Serialize this instance.
  virtual void        serialize ( XmlTree::Node &parent ) const;

protected:

  virtual ~LandModelEllipsoid();

private:

  void                _setRadii ( double equator, double polar );

  osg::EllipsoidModel *_ellipsoid;

  SERIALIZE_XML_CLASS_NAME ( LandModelEllipsoid );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAND_MODEL_ELLIPSOID_H__
