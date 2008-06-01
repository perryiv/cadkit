
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  ArcIMS layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAYER_ARC_IMS_H__
#define __MINERVA_CORE_RASTER_LAYER_ARC_IMS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayerNetwork.h"

#include <string>


namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayerArcIMS : public RasterLayerNetwork
{
public:

  typedef RasterLayerNetwork BaseClass;
  typedef BaseClass::IReadImageFile IReadImageFile;

  USUL_DECLARE_REF_POINTERS ( RasterLayerArcIMS );

  RasterLayerArcIMS ();

  /// Clone.
  virtual IUnknown*     clone() const;
  
protected:

  virtual ~RasterLayerArcIMS();
  
  RasterLayerArcIMS ( const RasterLayerArcIMS& );

  virtual void          _download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

private:

  // Do not use.
  RasterLayerArcIMS& operator = ( const RasterLayerArcIMS& );

  virtual std::string   _cacheFileExtension() const;

  std::string           _createRequestXml ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const;

  SERIALIZE_XML_CLASS_NAME ( RasterLayerArcIMS );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_ARC_IMS_H__
