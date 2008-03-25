
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

#ifndef __STAR_SYSTEM_RASTER_LAYER_WMS_H__
#define __STAR_SYSTEM_RASTER_LAYER_WMS_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include <map>
#include <string>


namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayerWms : public RasterLayer
{
public:

  typedef RasterLayer BaseClass;
  typedef std::map < std::string, std::string > Options;

  USUL_DECLARE_REF_POINTERS ( RasterLayerWms );

  RasterLayerWms ( const Extents &maxExtents = Extents ( -180, -90, 180, 90 ), const std::string &url = std::string(), const Options &options = Options() );
  
  /// Get/Set the cache directory.
  void                  cacheDirectory ( const std::string& dir, bool makeDefault = false );
  std::string           cacheDirectory() const;

  /// Clone.
  virtual IUnknown*     clone() const;

  /// Get/Set the default cache directory.
  static void           defaultCacheDirectory ( const std::string& );
  static std::string    defaultCacheDirectory();
  
  /// Set/get the options map.
  void                  options ( const Options& options );
  Options               options() const;
  
  /// Get the texture
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
  /// Set/get the url.
  void                  url ( const std::string& url );
  std::string           url() const;
  
protected:

  virtual ~RasterLayerWms();
  
  RasterLayerWms ( const RasterLayerWms& );

private:

  // Do not use.
  RasterLayerWms& operator = ( const RasterLayerWms& );

  std::string           _baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const;
  std::string           _baseFileName ( Extents extents ) const;

  std::string           _cacheDirectory() const;

  void                  _destroy();

  std::string           _getAllOptions() const;
  
  void                  _registerMembers();

  void                  _downloadFailed ( const std::string &file, const std::string &url );

  std::string _url;
  Options _options;
  std::string _dir;
  bool _useNetwork;
  bool _writeFailedFlags;
  bool _readFailedFlags;

  SERIALIZE_XML_DEFINE_MEMBERS ( RasterLayerWms );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __STAR_SYSTEM_RASTER_LAYER_WMS_H__
