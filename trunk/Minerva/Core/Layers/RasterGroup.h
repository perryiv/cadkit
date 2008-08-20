
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_RASTER_GROUP_H__
#define __MINERVA_CORE_RASTER_GROUP_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/RasterLayer.h"

#include "Minerva/Interfaces/IAddLayer.h"
#include "Minerva/Interfaces/IRemoveLayer.h"
#include "Minerva/Interfaces/ISwapLayers.h"

#include "Usul/Base/Object.h"
#include "Usul/Containers/Unknowns.h"
#include "Usul/Interfaces/IDataChangedListener.h"
#include "Usul/Interfaces/IDataChangedNotify.h"

#include "osg/Image"
#include "osg/ref_ptr"

#include <vector>
#include <map>

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT RasterGroup : public RasterLayer,
                                   public Minerva::Interfaces::IAddLayer,
                                   public Minerva::Interfaces::IRemoveLayer,
                                   public Minerva::Interfaces::ISwapLayers,
                                   public Usul::Interfaces::IDataChangedNotify
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
  typedef Usul::Interfaces::ILayer ILayer;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Usul::Interfaces::ITreeNode ITreeNode;

  USUL_DECLARE_QUERY_POINTERS ( RasterGroup );
  
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  RasterGroup();
  
  // Clone.
  virtual IUnknown*               clone() const;

  /// Deserialize.
  virtual                         void deserialize ( const XmlTree::Node &node );

  void                            append ( IRasterLayer* layer );
  void                            remove ( IRasterLayer* layer );
  void                            swap   ( IRasterLayer* layer0, IRasterLayer* layer1 );
  
  /// Get the i'th layer.
  virtual ILayer*                 layer ( unsigned int i );

  /// Get a copy of the layers.
  void                            layers ( Layers& layers ) const;

  // Get/set the log.
  virtual void                    logSet ( LogPtr );
  LogPtr                          logGet();

  unsigned int                    size() const;

  virtual ImagePtr                texture ( const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job *, IUnknown *caller );

protected:

  virtual ~RasterGroup();

  RasterGroup ( const RasterGroup& );

  void                            _updateExtents ( IRasterLayer* layer );

  void                            _cacheAdd ( const Extents& extents, unsigned int width, unsigned int height, osg::Image *image );
  ImagePtr                        _cacheFind ( const Extents& extents, unsigned int width, unsigned int height ) const;

  virtual void                    _compositeImages ( osg::Image& result, const osg::Image& image, const RasterLayer::Alphas &alphas, float alpha, Usul::Jobs::Job * );

  static ImageKey                 _makeKey ( const Extents& extents, unsigned int width, unsigned int height );

  /// Notify data changed listeners.
  void                            _notifyDataChnagedListeners();
  
  // Get the number of children (ITreeNode).
  virtual unsigned int            getNumChildNodes() const;

  // Get the child node (ITreeNode).
  virtual ITreeNode *             getChildNode ( unsigned int which );

  // Set/get the name (ITreeNode).
  virtual void                    setTreeNodeName ( const std::string & );
  virtual std::string             getTreeNodeName() const;

  // Set the state (IBooleanState).
  virtual void                    setBooleanState ( bool );
  
  // Add a layer (IAddLayer).
  virtual void                    addLayer ( Usul::Interfaces::IUnknown *layer );

  /// Remove a layer (IRemoveLayer).
  virtual void                    removeLayer ( Usul::Interfaces::IUnknown * layer );
  
  /// Swap layers (ISwapLayers).
  virtual void                    swapLayers ( Usul::Interfaces::IUnknown *layer0, Usul::Interfaces::IUnknown* layer1 );

  // Add the listener.
  virtual void                    addDataChangedListener ( Usul::Interfaces::IUnknown *caller );
  
  // Remove the listener.
  virtual void                    removeDataChangedListener ( Usul::Interfaces::IUnknown *caller );
  
private:
  RasterGroup& operator= ( const RasterGroup& );

  typedef Usul::Interfaces::IDataChangedListener      IDataChangedListener;
  typedef Usul::Containers::Unknowns<IDataChangedListener> DataChangedListeners;
  
  Layers _layers;
  ImageCache _cache;
  bool _useCache;
  DataChangedListeners _dataChangedListeners;
  
  SERIALIZE_XML_CLASS_NAME( RasterGroup );
};

} // namespace Layers
} // namespace Core
} // namespace Minerva

#endif // __MINERVA_CORE_RASTER_GROUP_H__
