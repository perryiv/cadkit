
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

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/File/Log.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IClonable.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ILayerExtents.h"
#include "Usul/Interfaces/IRasterAlphas.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/IReadImageFile.h"
#include "Usul/Interfaces/ISerialize.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Math/Vector3.h"

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


class MINERVA_EXPORT RasterLayer : public Usul::Base::Object,
                                   public Usul::Interfaces::IClonable,
                                   public Usul::Interfaces::ILayer,
                                   public Usul::Interfaces::ILayerExtents,
                                   public Usul::Interfaces::IRasterAlphas,
                                   public Usul::Interfaces::IRasterLayer,
                                   public Usul::Interfaces::ISerialize,
                                   public Usul::Interfaces::ITreeNode,
                                   public Usul::Interfaces::IBooleanState
{
public:

  typedef Usul::Base::Object BaseClass;
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef osg::ref_ptr < osg::Image > ImagePtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::IRasterAlphas::Alphas Alphas;
  typedef Usul::Interfaces::IReadImageFile IReadImageFile;
  typedef IReadImageFile::RefPtr ReaderPtr;
  typedef Usul::File::Log::RefPtr LogPtr;

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
  void                  cacheDirectory ( const std::string& dir, bool makeDefault = false );
  std::string           cacheDirectory() const;

  // Clone this layer.
  virtual IUnknown*     clone() const = 0;

  /// Get/Set the default cache directory.
  static void           defaultCacheDirectory ( const std::string& );
  static std::string    defaultCacheDirectory();

  /// Get/Set the extents.
  void                  extents ( const Extents& extents );
  Extents               extents () const;

  /// Get the texture.
  virtual ImagePtr      texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

  /// Get the guid for the layer.
  virtual std::string   guid() const;

  // Set/get the log.
  virtual void          logSet ( LogPtr );
  LogPtr                logGet();

  /// Get/Set the name.
  virtual std::string   name() const;
  virtual void          name( const std::string& );

  /// Get/Set show layer
  virtual void          showLayer ( bool b );
  virtual bool          showLayer() const;

protected:

  virtual ~RasterLayer();
  
  RasterLayer ( const RasterLayer& );

  std::string           _baseDirectory ( const std::string &cacheDir, unsigned int width, unsigned int height, unsigned int level ) const;
  std::string           _baseFileName ( Extents extents ) const;
  static std::string    _buildCacheDir ( const std::string &rootDir, const std::string &mangledUrl, const std::size_t hashValue );

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

  static std::string    _mangledURL ( const std::string &url );

  virtual ImagePtr      _readImageFile ( const std::string & ) const;
  static ImagePtr       _readImageFile ( const std::string &, ReaderPtr );

  void                  _writeImageToCache ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, ImagePtr );

  /// Get the min latitude and min longitude (ILayerExtents).
  virtual double        minLon() const;
  virtual double        minLat() const;
 
  /// Get the max latitude and max longitude (ILayerExtents).
  virtual double        maxLon() const;
  virtual double        maxLat() const;
  
  // Get the number of children (ITreeNode).
  virtual unsigned int  getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode *   getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void          setTreeNodeName ( const std::string & );
  virtual std::string   getTreeNodeName() const;
  
  // Set/get the state (IBooleanState).
  virtual void          setBooleanState ( bool );
  virtual bool          getBooleanState() const;
  
private:

  // Do not use.
  RasterLayer& operator= ( const RasterLayer& );
  
  // Register members for serialization.
  void                  _registerMembers();

  Extents _extents;
  Alphas _alphas;
  std::string _guid;
  bool _shown;
  float _alpha;
  std::string _cacheDir;
  IReadImageFile::RefPtr _reader;
  LogPtr _log;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( RasterLayer );
};


} // namespace Layers
} // namespace Core
} // namespace Minerva


#endif // __MINERVA_CORE_RASTER_LAYER_H__
