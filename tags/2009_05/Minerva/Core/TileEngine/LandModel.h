
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAND_MODEL_H__
#define __MINERVA_CORE_RASTER_LAND_MODEL_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"

#include "osg/Vec2d"

namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT LandModel : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef Usul::Math::Matrix44d Matrix;

  USUL_DECLARE_REF_POINTERS ( LandModel );

  LandModel () : BaseClass(),
    SERIALIZE_XML_INITIALIZER_LIST
  {
  }

  // Convert lat, lon, height to x,y,z.
  virtual void        latLonHeightToXYZ ( double lat, double lon, double elevation, double& x, double& y, double& z ) const = 0;
  virtual void        xyzToLatLonHeight ( double x, double y, double z, double& lat, double& lon, double& elevation ) const = 0;

  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual Matrix      planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const = 0;

  // Return the appropriate mesh size.
  virtual MeshSize    meshSize ( const Extents &extents, const MeshSize &ms ) = 0;

protected:

  virtual ~LandModel()
  {
  }

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( LandModel );
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAND_MODEL_H__
