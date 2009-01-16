
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

#include "Minerva/Layers/GDAL/Export.h"

#include "Minerva/Core/Layers/RasterLayer.h"

#include "Minerva/Layers/PostGIS/Layer.h"
#include "Minerva/Layers/PostGIS/PolygonLayer.h"

#include "Usul/File/Temp.h"

class GDALDataset;
class OGRGeometry;


namespace Minerva {

class MINERVA_GDAL_EXPORT RasterPolygonLayer : public Minerva::Core::Layers::RasterLayer
{
public:
  typedef Minerva::Core::Layers::RasterLayer BaseClass;
  typedef Minerva::Layers::PostGIS::Layer Layer;
  typedef Minerva::Layers::PostGIS::PolygonLayer PolygonLayer;
  
  RasterPolygonLayer();
  RasterPolygonLayer ( Layer* layer );
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );

protected:
  
  virtual ~RasterPolygonLayer();
  
  RasterPolygonLayer ( const RasterPolygonLayer& );

  void                  _init();
  void                  _initGeometries();

  /// Rasterize.
  ImagePtr              _rasterize ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level );
  
  /// Cache functions.
  std::string           _baseFileName ( Extents extents ) const;
  std::string           _directory ( unsigned int width, unsigned int height, unsigned int level ) const;

  /// Read/Write image file.
  ImagePtr              _readImageFile ( const std::string& filename ) const;
  void                  _writeImageFile ( ImagePtr image, const std::string& filename ) const;
  
private:

  RasterPolygonLayer& operator= ( const RasterPolygonLayer& );
  
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

  
}

#endif // __MINERVA_RASTER_POLYGON_LAYER_H__
