
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
#include "Minerva/Core/TileEngine/Node.h"
#include "Minerva/Core/TileEngine/LandModel.h"
#include "Minerva/Core/TileEngine/SplitCallbacks.h"
#include "Minerva/Core/TileEngine/Tile.h"
#include "Minerva/Core/Jobs/BuildRaster.h"
#include "Minerva/Core/Layers/ElevationGroup.h"
#include "Minerva/Core/Layers/RasterGroup.h"
#include "Minerva/Core/Layers/VectorGroup.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Interfaces/IElevationDatabase.h"
#include "Usul/Interfaces/IFrameStamp.h"
#include "Usul/Interfaces/IPlanetCoordinates.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Variable.h"

#include "boost/shared_ptr.hpp"

#include "osg/Vec2d"
#include "osg/MatrixTransform"
#include "osg/Texture"

#include <list>


namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT Body : public Node,
                            public Usul::Interfaces::IUpdateListener,
                            public Usul::Interfaces::IPlanetCoordinates,
                            public Usul::Interfaces::IElevationDatabase,
                            public Usul::Interfaces::IFrameStamp,
                            public Usul::Interfaces::ITreeNode
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef Minerva::Core::TileEngine::Callbacks::SplitCallback SplitCallback;
  typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;
  typedef Usul::Pointers::WeakPointer < Body > WeakPtr;
  typedef Minerva::Core::Layers::VectorGroup VectorGroup;
  typedef Minerva::Core::Layers::ElevationGroup ElevationGroup;
  typedef Minerva::Core::Layers::RasterGroup RasterGroup;
  typedef Minerva::Core::Layers::RasterLayer RasterLayer;
  typedef Minerva::Core::Layers::RasterGroup::Layers Rasters;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::list<Tile::RefPtr> Tiles;
  typedef Minerva::Core::Jobs::BuildRaster BuildRaster;

  // Helper macro for repeated code.
  MINERVA_DEFINE_NODE_CLASS ( Body );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;
  
  // Constructors
  Body ( LandModel *land = 0x0, Usul::Jobs::Manager *manager = 0x0, const MeshSize &ms = MeshSize ( 17, 17 ), double splitDistance = 1 );

  // Add a tile for the given extents.
  void                      addTile ( const Extents & );
  
  // Set/get the flag that says to allow spliting.
  void                      allowSplitting ( bool );
  bool                      allowSplitting() const;

  // Set/get the flag that says to cache the tiles.
  bool                      cacheTiles() const;
  void                      cacheTiles ( bool );

  // Set/get the center.
  void                      center ( const Vec3d & );
  Vec3d                     center() const;

  // Deserialize this instance.
  virtual void              deserialize ( const XmlTree::Node &node );

  // Dirty textures.
  void                      dirtyTextures ( const Extents& e );

  // Get the elevation data.
  RasterLayer::RefPtr       elevationData();

  // Get the elevation at a lat, lon.
  double                    elevation ( double lat, double lon ) const;

  // Get the elevation at a lat, lon (IElevationDatabase).
  virtual double            elevationAtLatLong ( double lat, double lon ) const;
  
  // Append elevation data.
  void                      elevationAppend ( Usul::Interfaces::IRasterLayer * );

  // Set/get the job manager for this body.
  void                      jobManager ( Usul::Jobs::Manager * );
  Usul::Jobs::Manager *     jobManager();

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const;
  void                      xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const;

  /// Convert to planet coordinates.
  virtual void              convertToPlanet ( const Usul::Math::Vec3d& orginal, Usul::Math::Vec3d& planetPoint ) const;
  virtual void              convertFromPlanet ( const Usul::Math::Vec3d& planetPoint, Usul::Math::Vec3d& latLonPoint ) const;
  
  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  virtual osg::Matrixd      planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;
  
  // Set/get the maximum level.
  void                      maxLevel ( unsigned int level );
  unsigned int              maxLevel() const;

  // Return the mesh size for the extents.
  MeshSize                  meshSize ( const Body::Extents &extents );

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

  // See if the tile should split.
  bool                      shouldSplit ( bool suggestion, Tile * );

  // Set/get the callback used for tile splitting. Returns the existing callback.
  SplitCallback::RefPtr     splitCallback() const;
  void                      splitCallback ( SplitCallback * );

  /// Get/Set the split distance.
  void                      splitDistance ( double distance, bool children = true );
  double                    splitDistance() const;

  // Request texture.
  BuildRaster::RefPtr       textureRequest ( Tile* );

  // Set/get the flag that says to use skirts.
  void                      useSkirts ( bool );
  bool                      useSkirts() const;

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

  void                      _addTileToBeDeleted ( Tile *tile );

  Extents                   _buildExtents ( Usul::Interfaces::IUnknown* unknown );

  void                      _addUpdateListener ( IUnknown *caller );
  void                      _removeUpdateListener ( IUnknown *caller );
  
  // Get the frame stamp (IFrameStamp). (This is a hack for layers that need to update every n seconds.)
  virtual osg::FrameStamp *                frameStamp();
  virtual const osg::FrameStamp *          frameStamp() const;
  
  // Get the number of children (ITreeNode).
  virtual unsigned int                     getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode *                      getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                             setTreeNodeName ( const std::string & );
  virtual std::string                      getTreeNodeName() const;
  
private:

  friend class Tile;

  typedef std::map < unsigned long, BuildRaster::RefPtr > TextureJobs;
  typedef Usul::Threads::Variable<Usul::Jobs::Manager*> JobManager;
  typedef Usul::Interfaces::IUpdateListener IUpdateListener;
  typedef std::vector<IUpdateListener::RefPtr> UpdateListeners;

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  MatrixTransformPtr _transform;
  LandModel::RefPtr _landModel;
  RasterGroup::RefPtr _rasters;
  ElevationGroup::RefPtr _elevation;
  VectorGroup::RefPtr _vectorData;
  JobManager _manager;
  TextureJobs _textureJobs;
  unsigned int _maxLevel;
  bool _cacheTiles;
  double _splitDistance;
  MeshSize _meshSize;
  bool _useSkirts;
  SplitCallback::RefPtr _splitCallback;
  double _scale;
  Tiles _deleteTiles;
  Tiles _topTiles;
  UpdateListeners _updateListeners;
  bool _allowSplitting;

  SERIALIZE_XML_CLASS_NAME ( Body );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_TILE_ENGINE_BODY_CLASS_H_
