
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

#include "osg/Image"
#include "osg/ref_ptr"

#include <vector>
#include <map>

namespace StarSystem {

class STAR_SYSTEM_EXPORT RasterGroup : public RasterLayer
{
public:

  typedef RasterLayer BaseClass;
  typedef std::vector < RasterLayer::RefPtr > Layers;
  typedef osg::ref_ptr < osg::Image > ImagePtr;
  typedef std::pair < unsigned int, unsigned int > KeySize;
  typedef std::pair < Extents::value_type, Extents::value_type > KeyRange;
  typedef std::pair < KeyRange, KeyRange > KeyBounds;
  typedef std::pair < KeySize, KeyBounds > ImageKey;
  typedef std::map < ImageKey, ImagePtr > ImageCache;

  USUL_DECLARE_REF_POINTERS ( RasterGroup );

  RasterGroup();

  void                            append ( RasterLayer* layer );

  virtual osg::Image*             texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * );

protected:

  virtual ~RasterGroup();

  void                            _updateExtents ( const RasterLayer& layer );

  void                            _cacheAdd ( const Extents& extents, unsigned int width, unsigned int height, osg::Image *image );
  osg::Image *                    _cacheFind ( const Extents& extents, unsigned int width, unsigned int height ) const;

  static void                     _compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, Usul::Jobs::Job * );

  static ImageKey                 _makeKey ( const Extents& extents, unsigned int width, unsigned int height );

private:

  Layers _layers;
  ImageCache _cache;
  bool _useCache;
};

}

#endif // __STAR_SYSTEM_RASTER_GROUP_H__
