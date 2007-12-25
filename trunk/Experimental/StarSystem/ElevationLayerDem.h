
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__
#define __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__

#include "StarSystem/Export.h"
#include "StarSystem/RasterLayerOssim.h"

#include "Usul/Math/Vector2.h"

class ossimProjection; class ossimDemGrid;

namespace StarSystem {


class STAR_SYSTEM_EXPORT ElevationLayerDem : public RasterLayer
{
public:
  typedef RasterLayer BaseClass;

  USUL_DECLARE_REF_POINTERS ( ElevationLayerDem );

  ElevationLayerDem();
  
  /// Open the dem.
  void                  open ( const std::string& filename );

  /// Get the texture
  virtual osg::Image*   texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * );

  // Get the value at the lat, lon location.  May return null pixel value.
  double                value ( double lon, double lat ) const;
  
protected:
  virtual ~ElevationLayerDem();

  virtual osg::Image*   _createBlankImage ( unsigned int width, unsigned int height ) const;
  
private:
  bool _loaded;
  ossimDemGrid *_grid;
  ossimProjection *_projection;

  SERIALIZE_XML_DEFINE_MEMBERS ( ElevationLayerDem );
};


}

#endif // __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__
