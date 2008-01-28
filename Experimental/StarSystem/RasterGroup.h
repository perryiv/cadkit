
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
  typedef Usul::Interfaces::IRasterLayer IRasterLayer;
  typedef std::vector < IRasterLayer::QueryPtr > Layers;
  typedef osg::ref_ptr < osg::Image > ImagePtr;
  typedef std::pair < unsigned int, unsigned int > KeySize;
  typedef std::pair < Extents::value_type, Extents::value_type > KeyRange;
  typedef std::pair < KeyRange, KeyRange > KeyBounds;
  typedef std::pair < KeySize, KeyBounds > ImageKey;
  typedef std::map < ImageKey, ImagePtr > ImageCache;
  typedef std::pair < ImagePtr, IRasterLayer::QueryPtr > RasterData;
  typedef std::vector < RasterData > Rasters;

  USUL_DECLARE_REF_POINTERS ( RasterGroup );

  RasterGroup();
  
  // Clone.
  virtual IUnknown*               clone() const;

  void                            append ( IRasterLayer* layer );
  void                            remove ( IRasterLayer* layer );

  IRasterLayer*                   layer ( unsigned int i );
 
  /// Get a copy of the layers.
  void                            layers ( Layers& layers ) const;

  unsigned int                    size() const;

  virtual ImagePtr                texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  void                            textures ( Rasters& rasters, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
protected:

  virtual ~RasterGroup();
  
  RasterGroup ( const RasterGroup& );
  RasterGroup& operator= ( const RasterGroup& );

  void                            _updateExtents ( IRasterLayer* layer );

  void                            _cacheAdd ( const Extents& extents, unsigned int width, unsigned int height, osg::Image *image );
  ImagePtr                        _cacheFind ( const Extents& extents, unsigned int width, unsigned int height ) const;

  virtual void                    _compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, float alpha, Usul::Jobs::Job * );

  static ImageKey                 _makeKey ( const Extents& extents, unsigned int width, unsigned int height );
  
private:

  Layers _layers;
  ImageCache _cache;
  bool _useCache;

  SERIALIZE_XML_DEFINE_MEMBERS ( RasterGroup );
};

}

#endif // __STAR_SYSTEM_RASTER_GROUP_H__
