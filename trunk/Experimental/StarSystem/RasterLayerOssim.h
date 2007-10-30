
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__
#define __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__

#include "StarSystem/Export.h"
#include "StarSystem/RasterLayer.h"

class ossimImageData;
class ossimImageHandler;
class ossimImageRenderer;
class ossimEquDistCylProjection;
class ossimViewInterface;

namespace StarSystem {


class STAR_SYSTEM_EXPORT RasterLayerOssim : public RasterLayer
{
public:
  typedef RasterLayer BaseClass;

  USUL_DECLARE_REF_POINTERS ( RasterLayerOssim );

  RasterLayerOssim();

  void                  open ( const std::string& );

  /// Get the texture
  virtual osg::Image*   texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level );

protected:
  virtual ~RasterLayerOssim();

  void                  _convert ( const ossimImageData& data, osg::Image& image ) const;
  void                  _updateExtents();

private:
  void                  _destroy();

  ossimImageHandler *_handler;
  ossimImageRenderer *_renderer;
  ossimViewInterface *_viewInterface;

  ossimEquDistCylProjection *_projection;
};


}

#endif // __STAR_SYSTEM_RASTER_LAYER_OSSIM_H__
