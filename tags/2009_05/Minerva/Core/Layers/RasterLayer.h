
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_LAYER_H__
#define __MINERVA_CORE_RASTER_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Feature.h"
#include "Minerva/Interfaces/ITileElevationData.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/File/Log.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/IRasterAlphas.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/IReadImageFile.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"

#include "osg/Vec2d"
#include "osg/Image"
#include "osg/ref_ptr"

#include <functional>
#include <map>

namespace osg { class Vec3d; }
namespace Usul { namespace Jobs { class Job; } }


namespace Minerva {
namespace Core {
namespace Layers {


class MINERVA_EXPORT RasterLayer : public Minerva::Core::Data::Feature,
                                   public Usul::Interfaces::IClonable,
                                   public Usul::Interfaces::ILayer,
                                   public Usul::Interfaces::IRasterAlphas,
                                   public Usul::Interfaces::IRasterLayer,
                                   public Usul::Interfaces::IBooleanState,
                                   public Minerva::Interfaces::ITileElevationData
{
public:

  typedef Minerva::Core::Data::Feature BaseClass;
  typedef BaseClass::Extents Extents;
  typedef osg::ref_ptr < osg::Image > ImagePtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IRasterAlphas::Alphas Alphas;
  typedef Usul::Interfaces::IReadImageFile IReadImageFile;
  typedef IReadImageFile::RefPtr ReaderPtr;
  typedef Usul::File::Log::RefPtr LogPtr;
  typedef Minerva::Interfaces::IElevationData IElevationData;

  USUL_DECLARE_QUERY_POINTERS ( RasterLayer );
  
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  RasterLayer();

  // Add an alpha value for the color, or an overall alpha.
  virtual void          alpha ( float );
  virtual void          alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );

  // Get the alpha values.
  virtual float         alpha() const;
  virtual Alphas        alphas() const;
  virtual void          alphas ( const Alphas& alphas );
    
  /// Get/set the cache directory.
  void                  baseCacheDirectory ( const std::string& dir, bool makeDefault = false );
  std::string           baseCacheDirectory() const;

  // Clone this layer.
  virtual IUnknown*     clone() const = 0;

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

  /// Get the guid for the layer.
  virtual std::string   guid() const;

  /// See if the given level falls within this layer's range of levels.
  bool                  isInLevelRange ( unsigned int level ) const;

  // Set/get the log.
  virtual void          logSet ( LogPtr );
  LogPtr                logGet();
  
  /// Get/Set the name (ILayer).
  virtual std::string   name() const;
  virtual void          name ( const std::string& );

  /// Get/Set show layer
  virtual void          showLayer ( bool b );
  virtual bool          showLayer() const;
  
  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );
  
protected:

  virtual ~RasterLayer();
  
  RasterLayer ( const RasterLayer& );

  std::string           _baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const;
  std::string           _baseFileName ( Extents extents ) const;
  static std::string    _buildCacheDir ( const std::string &rootDir, const std::string &mangledUrl, const std::size_t hashValue );

  enum CacheStatus
  {
    CACHE_STATUS_FILE_OK,
    CACHE_STATUS_FILE_DOES_NOT_EXIST,
    CACHE_STATUS_FILE_NAME_ERROR,
    CACHE_STATUS_FILE_EMPTY
  };

  CacheStatus           _getAndCheckCacheFilename ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, std::string& filename );
  virtual std::string   _cacheDirectory() const;
  virtual std::string   _cacheFileExtension() const;
  virtual std::string   _cacheFileName( const Extents& extents, unsigned int width, unsigned int height, unsigned int level ) const;
  static void           _checkForCanceledJob ( Usul::Jobs::Job *job );
  virtual ImagePtr      _createBlankImage ( unsigned int width, unsigned int height ) const;

  static std::size_t    _hashString ( const std::string &s );

  ReaderPtr             _imageReaderGet() const;
  void                  _imageReaderSet ( ReaderPtr );
  void                  _imageReaderFind ( const std::string &ext );

  void                  _logEvent ( const std::string &s );

  virtual ImagePtr      _readImageFile ( const std::string & ) const;
  static ImagePtr       _readImageFile ( const std::string &, ReaderPtr );

  void                  _writeImageToCache ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, ImagePtr );

  // Set/get the state (IBooleanState).
  virtual void          setBooleanState ( bool );
  virtual bool          getBooleanState() const;

private:

  // Do not use.
  RasterLayer& operator= ( const RasterLayer& );
  
  // Register members for serialization.
  void                  _registerMembers();

  Alphas _alphas;
  float _alpha;
  std::string _cacheDir;
  IReadImageFile::RefPtr _reader;
  LogPtr _log;
  Usul::Math::Vec2ui _levelRange;

  SERIALIZE_XML_CLASS_NAME( RasterLayer )
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_H__
