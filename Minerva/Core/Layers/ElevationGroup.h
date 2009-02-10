
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ELEVATION_GROUP_H__
#define __MINERVA_CORE_ELEVATION_GROUP_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterGroup.h"

namespace osg { class Image; }

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT ElevationGroup : public RasterGroup
{
public:

  typedef RasterGroup BaseClass;

  USUL_DECLARE_REF_POINTERS ( ElevationGroup );

  ElevationGroup();
  
  /// Clone.
  virtual IUnknown*     clone() const;

  /// Get the raster data as elevation data.
  virtual IElevationData::RefPtr elevationData ( 
    double minLon,
    double minLat,
    double maxLon,
    double maxLat,
    unsigned int width,
    unsigned int height,
    unsigned int level,
    Usul::Jobs::Job* job,
    Usul::Interfaces::IUnknown* caller );

protected:

  virtual ~ElevationGroup();

  ElevationGroup ( const ElevationGroup& );

  /// Get the raster data as elevation data.
  IElevationData::RefPtr _elevationData ( 
    const Layers& layers,
    double minLon,
    double minLat,
    double maxLon,
    double maxLat,
    unsigned int width,
    unsigned int height,
    unsigned int level,
    Usul::Jobs::Job* job,
    Usul::Interfaces::IUnknown* caller );

private:

  ElevationGroup& operator= ( const ElevationGroup& );
  
  SERIALIZE_XML_DEFINE_MEMBERS ( ElevationGroup );
};

} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __MINERVA_CORE_ELEVATION_GROUP_H__
