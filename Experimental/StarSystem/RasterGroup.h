
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __STAR_SYSTEM_RASTER_GROUP_H__
#define __STAR_SYSTEM_RASTER_GROUP_H__

#include "StarSystem/Export.h"
#include "StarSystem/RasterLayer.h"

#include "Usul/Base/Object.h"

#include <vector>

namespace StarSystem {

class STAR_SYSTEM_EXPORT RasterGroup : public RasterLayer
{
public:
  typedef RasterLayer BaseClass;
  typedef std::vector < RasterLayer::RefPtr > Layers;

  USUL_DECLARE_REF_POINTERS ( RasterGroup );

  RasterGroup();

  void                            append ( RasterLayer* layer );

  virtual osg::Image*             texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level );

protected:

  virtual ~RasterGroup();

  void                            _updateExtents ( const RasterLayer& layer );

  void                            _compositeImages ( osg::Image& result, const osg::Image& image ) const;

private:

  Layers _layers;
};

}

#endif // __STAR_SYSTEM_RASTER_GROUP_H__
