
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  WMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAYER_WMS_H__
#define __MINERVA_CORE_RASTER_LAYER_WMS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayerNetwork.h"

#include <map>
#include <string>


namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayerWms : public RasterLayerNetwork
{
public:

  typedef RasterLayerNetwork BaseClass;
  typedef std::map < std::string, std::string > Options;
  typedef BaseClass::IReadImageFile IReadImageFile;

  USUL_DECLARE_REF_POINTERS ( RasterLayerWms );

  RasterLayerWms ( const Extents &maxExtents = Extents ( -180, -90, 180, 90 ), const std::string &url = std::string(), const Options &options = Options() );

  /// Clone.
  virtual IUnknown*     clone() const;
  
protected:

  virtual ~RasterLayerWms();
  
  RasterLayerWms ( const RasterLayerWms& );

  virtual void          _download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

private:

  // Do not use.
  RasterLayerWms& operator = ( const RasterLayerWms& );

  SERIALIZE_XML_CLASS_NAME ( RasterLayerWms );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_WMS_H__
