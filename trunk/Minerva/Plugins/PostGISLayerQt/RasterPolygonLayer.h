
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

namespace Usul {
  namespace Factory {
    template < class T > class TypeCreator;
  }
}

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
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );
protected:
  
  RasterPolygonLayer();
  virtual ~RasterPolygonLayer();
  
  RasterPolygonLayer ( const RasterPolygonLayer& );
  RasterPolygonLayer& operator= ( const RasterPolygonLayer& );
  
  void _init();
  
private:
  friend class Usul::Factory::TypeCreator<RasterPolygonLayer>;
  
  PolygonLayer::RefPtr _layer;
  Usul::File::Temp _temp;
  GDALDataset *_data;
  
  SERIALIZE_XML_CLASS_NAME( RasterPolygonLayer ) 
  SERIALIZE_XML_SERIALIZE_FUNCTION 
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


#endif // __MINERVA_RASTER_POLYGON_LAYER_H__
