
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
//  Body class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_TILE_ENGINE_BODY_CLASS_H_
#define _MINERVA_CORE_TILE_ENGINE_BODY_CLASS_H_

#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Macros.h"
#include "Minerva/Core/TileEngine/LandModel.h"
#include "Minerva/Core/TileEngine/SplitCallbacks.h"
#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/Jobs/BuildRaster.h"
#include "Minerva/Core/Layers/ElevationGroup.h"
#include "Minerva/Core/Layers/RasterGroup.h"
#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/Utilities/SkyDome.h"

#include "Usul/Containers/Unknowns.h"
#include "Usul/File/Log.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IRasterAlphas.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Threads/Variable.h"

#include "boost/shared_ptr.hpp"

#include "osg/Vec2d"
#include "osg/MatrixTransform"
#include "osg/Texture"

#include <list>


namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT Body : public Usul::Base::Object,
                            public Usul::Interfaces::IUpdateListener,
                            public Usul::Interfaces::IPlanetCoordinates,
                            public Usul::Interfaces::IElevationDatabase,
                            public Usul::Interfaces::ITreeNode,
                            public Usul::Interfaces::IRasterAlphas
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef Minerva::Core::TileEngine::Callbacks::SplitCallback SplitCallback;
  typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;
  typedef Usul::Pointers::WeakPointer < Body > WeakPtr;
  typedef Minerva::Core::Data::Container VectorGroup;
  typedef Minerva::Core::Layers::ElevationGroup ElevationGroup;
  typedef Minerva::Core::Layers::RasterGroup RasterGroup;
  typedef Minerva::Core::Layers::RasterLayer RasterLayer;
  typedef Minerva::Core::Layers::RasterGroup::Layers Rasters;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::list<Tile::RefPtr> Tiles;
  typedef Minerva::Core::Jobs::BuildRaster BuildRaster;
  typedef Usul::File::Log::RefPtr LogPtr;
  typedef Usul::Math::Vec2ui ImageSize;

  // Helper macro for repeated code.
  MINERVA_DEFINE_NODE_CLASS ( Body );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  // Constructors
  Body ( LandModel *land = 0x0, Usul::Jobs::Manager *manager = 0x0, const MeshSize &ms = MeshSize ( 17, 17 ), double splitDistance = 1 );

  // Add a tile for the given extents.
  void                      addTile ( const Extents & );
  
  // Set/get the flag that says to allow spliting.
  void                      allowSplit ( bool );
  bool                      allowSplit() const;

  // Add an alpha value.
  virtual void              alpha ( float );
  virtual void              alpha ( unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha );

  // Get the alpha values.
  virtual float             alpha() const;
  virtual Alphas            alphas() const;
  
  // Set/get the flag that says to cache the tiles.
  bool                      cacheTiles() const;
  void                      cacheTiles ( bool );

  // Set/get the center.
  void                      center ( const Vec3d & );
  Vec3d                     center() const;
  
  // Clear the body.
  void                      clear();

  // Deserialize this instance.
  virtual void              deserialize ( const XmlTree::Node &node );

  // Dirty textures and vertices.
  void                      dirtyTextures ( const Extents& e );
  void                      dirtyVertices();

  // Get the elevation data.
  RasterLayer::RefPtr       elevationData();

  // Get the elevation at a lat, lon.
  double                    elevation ( double lat, double lon ) const;

  // Get the elevation at a lat, lon (IElevationDatabase).
  virtual double            elevationAtLatLong ( double lat, double lon ) const;
  
  // Append elevation data.
  void                      elevationAppend ( Usul::Interfaces::IRasterLayer * );

  // Return overall extents of the body.
  Extents                   extents() const;
  
  // Intersect only with the tiles (no vector data).
  bool                      intersectWithTiles ( const Usul::Math::Vec3d& pt0, const Usul::Math::Vec3d& pt1, Usul::Math::Vec3d& point );

  // Set/get the job manager for this body.
  void                      jobManager ( Usul::Jobs::Manager * );
  Usul::Jobs::Manager *     jobManager();

  // Set/get the flag that says to keep detail.
  void                      keepDetail ( bool );
  bool                      keepDetail() const;

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const;
  void                      xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const;

  /// Convert to planet coordinates.
  virtual void              convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const;
  virtual void              convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& lonLatPoint ) const;
  
  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual osg::Matrixd      planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;

  // Get the land model.
  LandModel*                landModel() const;
  
  // Set/get the log.
  void                      logSet ( LogPtr );
  LogPtr                    logGet();

  // Get the max degree of anisotropic filtering.
  // See http://en.wikipedia.org/wiki/Anisotropic_filtering
  float                     maxAnisotropy() const;

  // Set/get the maximum level.
  void                      maxLevel ( unsigned int level );
  unsigned int              maxLevel() const;

  // Return the mesh size for the extents.
  MeshSize                  meshSize ( const Body::Extents &extents );
  
  // Set/get the needs redraw state.
  void                      needsRedraw ( bool b );
  bool                      needsRedraw() const;
  
  // Get the number of new textures last frame.
  unsigned int              newTexturesLastFrame() const;

  // Pre- and post-render notifications.
  virtual void              preRender  ( Usul::Interfaces::IUnknown *caller );
  virtual void              postRender ( Usul::Interfaces::IUnknown *caller );

  // Purge tiles that are ready.
  void                      purgeTiles();

  // Get the rasters.
  void                      rasters ( Rasters& rasters ) const;

  // Append raster data.
  void                      rasterAppend ( Usul::Interfaces::IRasterLayer * );
  
  // Remove raster layer
  void                      rasterRemove ( Usul::Interfaces::IRasterLayer * );

  // Raster has chagned.
  void                      rasterChanged (  Usul::Interfaces::IRasterLayer * );
  
  // Get the raster data.
  RasterLayer::RefPtr       rasterData();

  // Get/Set the scale for the system.
  void                      scale( double );
  double                    scale() const;

  // Get the scene.
  const osg::Node *         scene() const;
  osg::Node *               scene();

  // Serialize this instance.
  virtual void              serialize ( XmlTree::Node &parent ) const;

  // Set the intersection.
  void                      intersectionGraphicClear();
  void                      intersectionGraphicSet ( double lonMouse, double latMouse, double elevMouse, double lonPoint, double latPoint, double elevPoint );

  // See if the tile should split.
  bool                      shouldSplit ( bool suggestion, Tile * );

  // Set/get the callback used for tile splitting. Returns the existing callback.
  SplitCallback::RefPtr     splitCallback();
  void                      splitCallback ( SplitCallback * );

  /// Get/Set the split distance.
  void                      splitDistance ( double distance, bool children = true );
  double                    splitDistance() const;

  // Request texture.
  BuildRaster::RefPtr       textureRequest ( Tile* );

  // Set/get the flag that says to use borders.
  void                      useBorders ( bool );
  bool                      useBorders() const;

  // Set/get the flag that says to use skirts.
  void                      useSkirts ( bool );
  bool                      useSkirts() const;

  // Update the tile's alpha.
  void                      updateTilesAlpha();

  /// Update (Usul::Interfaces::IUpdateListener).
  virtual void              updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Get the vector data.
  VectorGroup::RefPtr       vectorData();
  
  /// Add vector data.
  void                      vectorAppend ( Usul::Interfaces::IUnknown *unknown );
  
  /// Remove vector data.
  void                      vectorRemove ( Usul::Interfaces::IUnknown *unknown );
  
protected:

  // Use reference counting.
  virtual ~Body();

  void                      _addTileToBeDeleted ( Tile::RefPtr tile );

  Extents                   _buildExtents ( Usul::Interfaces::IUnknown* unknown );

  void                      _addUpdateListener ( IUnknown *caller );
  void                      _removeUpdateListener ( IUnknown *caller );

  void                      _updateTileAlpha ( osg::Group *group );
  
  // Get the number of children (ITreeNode).
  virtual unsigned int                     getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr                getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                             setTreeNodeName ( const std::string & );
  virtual std::string                      getTreeNodeName() const;
  
private:

  friend class Tile;

  typedef Usul::Threads::Variable<Usul::Jobs::Manager*> JobManager;
  typedef Usul::Interfaces::IUpdateListener IUpdateListener;
  typedef Usul::Containers::Unknowns<IUpdateListener> UpdateListeners;

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();
  void                      _textureAdded();
  
  void                      _dirtyTiles ( unsigned int flags );
  void                      _dirtyTiles ( unsigned int flags, const Extents& extents );

  MatrixTransformPtr _transform;
  VectorGroup::RefPtr _graphic;
  LandModel::RefPtr _landModel;
  RasterGroup::RefPtr _rasters;
  ElevationGroup::RefPtr _elevation;
  VectorGroup::RefPtr _vectorData;
  JobManager _manager;
  unsigned int _maxLevel;
  bool _cacheTiles;
  double _splitDistance;
  MeshSize _meshSize;
  bool _useSkirts;
  bool _useBorders;
  SplitCallback::RefPtr _splitCallback;
  double _scale;
  Tiles _deleteTiles;
  Tiles _topTiles;
  UpdateListeners _updateListeners;
  bool _allowSplit;
  bool _keepDetail;
  Minerva::Core::Utilities::SkyDome::RefPtr _sky;
  unsigned int _newTexturesLastFrame;
  bool _needsRedraw;
  LogPtr _log;
  std::string _name;
  ImageSize _imageSize;
  float _alpha;
  float _maxAnisotropy;

  SERIALIZE_XML_CLASS_NAME ( Body );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
  SERIALIZE_XML_DEFINE_MAP;
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_TILE_ENGINE_BODY_CLASS_H_
