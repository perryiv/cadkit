
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
//  Network layer class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAYER_NETWORK_H__
#define __MINERVA_CORE_RASTER_LAYER_NETWORK_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include <map>
#include <string>


namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayerNetwork : public RasterLayer
{
public:

  typedef RasterLayer BaseClass;
  typedef std::map < std::string, std::string > Options;
  typedef BaseClass::IReadImageFile IReadImageFile;

  USUL_DECLARE_REF_POINTERS ( RasterLayerNetwork );

  RasterLayerNetwork ( const Extents &maxExtents = Extents ( -180, -90, 180, 90 ), const std::string &url = std::string(), const Options &options = Options() );
  
  /// The cache directory.
  std::string           cacheDirectory() const;
  
  /// The cache file extension.
  std::string           cacheFileExtension() const;
  
  /// Deserialize.
  virtual void          deserialize ( const XmlTree::Node& node );
  
  /// Set/get the options map.
  void                  options ( const Options& options );
  Options               options() const;
  
  /// Get the texture
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

  /// Set/get the max number of attempts.
  void                  maxNumAttempts ( unsigned int );
  unsigned int          maxNumAttempts() const;

  /// Set/get the timeout in milliseconds.
  void                  timeoutMilliSeconds ( unsigned int );
  unsigned int          timeoutMilliSeconds() const;
  
  /// Set/get the url.
  void                  urlBase ( const std::string& url );
  std::string           urlBase() const;

  /// Get the full url.
  virtual std::string   urlFull ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const;

	/// Should this layer use the network?
	bool                  useNetwork() const;
  
protected:

  virtual ~RasterLayerNetwork();
  
  RasterLayerNetwork ( const RasterLayerNetwork& );

  std::string           _getAllOptions() const;

  virtual void          _download ( const std::string& file, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller ) = 0;

private:

  // Do not use.
  RasterLayerNetwork& operator = ( const RasterLayerNetwork& );

  virtual std::string   _cacheDirectory() const;
  virtual std::string   _cacheFileExtension() const;

  void                  _destroy();
  
  void                  _findImageReader();

  void                  _registerMembers();

  void                  _downloadFailed ( const std::string &file, const std::string &url );

  std::string _url;
  Options _options;
  bool _useNetwork;
  bool _writeFailedFlags;
  bool _readFailedFlags;
  unsigned int _maxNumAttempts;
  unsigned int _timeout;

  SERIALIZE_XML_CLASS_NAME ( RasterLayerNetwork );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_NETWORK_H__
