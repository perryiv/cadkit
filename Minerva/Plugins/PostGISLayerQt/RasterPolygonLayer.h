
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_RASTER_POLYGON_LAYER_H__
#define __MINERVA_RASTER_POLYGON_LAYER_H__

#include "StarSystem/RasterLayerOssim.h"

#include "Minerva/Core/Layers/Layer.h"
#include "Minerva/Core/Layers/PolygonLayer.h"

#include "Usul/File/Temp.h"

class GDALDataset;

class RasterPolygonLayer : public StarSystem::RasterLayerOssim
{
public:
  typedef StarSystem::RasterLayerOssim BaseClass;
  typedef Minerva::Core::Layers::Layer Layer;
  typedef Minerva::Core::Layers::PolygonLayer PolygonLayer;
  
  RasterPolygonLayer ( Layer* layer );
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
protected:
  virtual ~RasterPolygonLayer();
  
  RasterPolygonLayer ( const RasterPolygonLayer& );
  RasterPolygonLayer& operator= ( const RasterPolygonLayer& );
  
  void _init();
  
private:
  PolygonLayer::RefPtr _layer;
  Usul::File::Temp _temp;
  GDALDataset *_data;
};


#endif // __MINERVA_RASTER_POLYGON_LAYER_H__
