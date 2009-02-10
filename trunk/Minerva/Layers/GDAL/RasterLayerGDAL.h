
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_GDAL_LAYER_H__
#define __MINERVA_GDAL_LAYER_H__

#include "Minerva/Layers/GDAL/Export.h"
#include "Minerva/Layers/GDAL/Dataset.h"

#include "Minerva/Core/Layers/RasterLayer.h"

#include "Usul/Interfaces/IRead.h"

#include "gdalwarper.h"

class GDALDataset;

namespace Minerva {
namespace Layers {
namespace GDAL {

class MINERVA_GDAL_EXPORT RasterLayerGDAL : public Minerva::Core::Layers::RasterLayer,
                                            public Usul::Interfaces::IRead
{
public:
  typedef Minerva::Core::Layers::RasterLayer BaseClass;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::vector<double> GeoTransform;
  
  USUL_DECLARE_QUERY_POINTERS ( RasterLayerGDAL );
  
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  RasterLayerGDAL();
  
  /// Clone.
  virtual IUnknown*     clone() const;
  
  /// Read.
  virtual void          read ( const std::string& filename, Usul::Interfaces::IUnknown* caller = 0x0, Usul::Interfaces::IUnknown* progrss = 0x0 );
  
  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node &node );
  
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
  
  virtual ~RasterLayerGDAL();
  
  RasterLayerGDAL ( const RasterLayerGDAL& );

  static GDALWarpOptions* _createWarpOptions ( GDALDataset* src, GDALDataset* dst, int numBands );

  virtual std::string   _cacheDirectory() const;
  virtual std::string   _cacheFileExtension() const;

  /// Create dataset for the given extents.
  Dataset::RefPtr _createTile ( const std::string& filename, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

  void                  _print ( GDALDataset * data );

private:
  
  // No assignment.
  RasterLayerGDAL& operator= ( const RasterLayerGDAL& );
  
  GDALDataset *_data;
  std::string _filename;
  
  SERIALIZE_XML_CLASS_NAME( RasterLayerGDAL ) 
};

}
}
}

#endif // __MINERVA_GDAL_LAYER_H__
