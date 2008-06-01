
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
  
  /// Set/get the url.
  void                  url ( const std::string& url );
  std::string           url() const;
  
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

  SERIALIZE_XML_CLASS_NAME ( RasterLayerNetwork );
  SERIALIZE_XML_SERIALIZE_FUNCTION
  SERIALIZE_XML_ADD_MEMBER_FUNCTION
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_NETWORK_H__
