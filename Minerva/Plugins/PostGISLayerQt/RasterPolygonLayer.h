
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
class OGRGeometry;

namespace Usul {
  namespace Factory {
    template < class T > class TypeCreator;
  }
}

class RasterPolygonLayer : public StarSystem::RasterLayer
{
public:
  typedef StarSystem::RasterLayer BaseClass;
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
  
  void _init();
  void _initGeometries();
  
  /// Rasterize.
  ImagePtr              _rasterize ( const std::string& filename, const Extents& extents, unsigned int width, unsigned int height, unsigned int level );
  
  /// Cache functions.
  std::string           _baseFileName ( Extents extents ) const;
  std::string           _directory ( unsigned int width, unsigned int height, unsigned int level ) const;
  
private:
  RasterPolygonLayer& operator= ( const RasterPolygonLayer& );
  
  friend class Usul::Factory::TypeCreator<RasterPolygonLayer>;

  typedef std::vector<OGRGeometry*> Geometries;
  typedef std::vector<double> BurnValues;
  
  PolygonLayer::RefPtr _layer;
  std::string _dir;
  int _srid;
  std::string _projectionText;
  std::string _latLonProjectionText;
  bool _initialized;
  Geometries _geometries;
  BurnValues _burnValues;
  
  SERIALIZE_XML_CLASS_NAME( RasterPolygonLayer ) 
  SERIALIZE_XML_SERIALIZE_FUNCTION 
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


#endif // __MINERVA_RASTER_POLYGON_LAYER_H__
