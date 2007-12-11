
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


namespace StarSystem {


class STAR_SYSTEM_EXPORT ElevationLayerDem : public RasterLayerOssim
{
public:
  typedef RasterLayerOssim BaseClass;

  USUL_DECLARE_REF_POINTERS ( ElevationLayerDem );

  ElevationLayerDem();

  /// Get the texture
  virtual osg::Image*   texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * );

protected:
  virtual ~ElevationLayerDem();

  virtual osg::Image*   _createBlankImage ( unsigned int width, unsigned int height ) const;
  virtual void          _convert ( const ossimImageData& data, osg::Image& image );

};


}

#endif // __STAR_SYSTEM_ELEVATION_LAYER_DEM_H__
