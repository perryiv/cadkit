
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

#ifndef _STAR_SYSTEM_BODY_CLASS_H_
#define _STAR_SYSTEM_BODY_CLASS_H_

#include "StarSystem/Node.h"
#include "StarSystem/Extents.h"
#include "StarSystem/CutImageJob.h"
#include "StarSystem/ElevationGroup.h"
#include "StarSystem/LandModel.h"
#include "StarSystem/RasterGroup.h"
#include "StarSystem/SplitCallbacks.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Variable.h"

#include "boost/shared_ptr.hpp"

#include "osg/Vec2d"
#include "osg/MatrixTransform"
#include "osg/Texture"

namespace StarSystem { class Tile; }


namespace StarSystem {


class STAR_SYSTEM_EXPORT Body : public Node
{
public:

  // Useful typedefs.
  typedef Node BaseClass;
  typedef BaseClass::BuildOptions BuildOptions;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef StarSystem::Callbacks::SplitCallback SplitCallback;
  typedef osg::ref_ptr<osg::MatrixTransform> MatrixTransformPtr;
  typedef Usul::Pointers::WeakPointer < Body > WeakPtr;
  typedef RasterGroup::Rasters Rasters;
  typedef Usul::Interfaces::IUnknown IUnknown;

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Body );

  // Constructors
  Body ( LandModel *land = 0x0, Usul::Jobs::Manager *manager = 0x0, const MeshSize &ms = MeshSize ( 17, 17 ), double splitDistance = 1 );

  // Add a tile for the given extents.
  void                      addTile ( const Extents & );

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

  // Append elevation data.
  void                      elevationAppend ( Usul::Interfaces::IRasterLayer * );

  // Set/get the job manager for this body.
  void                      jobManager ( Usul::Jobs::Manager * );
  Usul::Jobs::Manager *     jobManager();

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const;
  void                      xyzToLatLonHeight ( const osg::Vec3d& point, double& lat, double& lon, double& elevation ) const;

  // Matrix to place items on the planet (i.e. local coordinates to world coordinates).
  osg::Matrixd              planetRotationMatrix ( double lat, double lon, double elevation, double heading ) const;
  
  // Set/get the maximum level.
  void                      maxLevel ( unsigned int level );
  unsigned int              maxLevel() const;

  // Return the mesh size for the extents.
  MeshSize                  meshSize ( const Body::Extents &extents );

  // Pre- and post-render notifications.
  virtual void              preRender  ( Usul::Interfaces::IUnknown *caller );
  virtual void              postRender ( Usul::Interfaces::IUnknown *caller );
  
  // Get the rasters.
  void                      rasters ( Rasters& rasters, const Extents& extents, unsigned int width, unsigned int height, unsigned int level, Usul::Jobs::Job * job, IUnknown *caller );

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
  void                      splitDistance ( double distance, bool dirty = true );
  double                    splitDistance() const;

  // Request texture.
  CutImageJob::RefPtr       textureRequest ( const Extents &extents, unsigned int level );

  // Set/get the flag that says to use skirts.
  void                      useSkirts ( bool );
  bool                      useSkirts() const;

protected:

  // Use reference counting.
  virtual ~Body();
  
  Extents                   _buildExtents ( Usul::Interfaces::IUnknown* unknown );

private:

  typedef std::map < unsigned long, CutImageJob::RefPtr > TextureJobs;
  typedef Usul::Threads::Variable<Usul::Jobs::Manager*> JobManager;

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  MatrixTransformPtr _transform;
  LandModel::RefPtr _landModel;
  RasterGroup::RefPtr _rasters;
  ElevationGroup::RefPtr _elevation;
  JobManager _manager;
  TextureJobs _textureJobs;
  unsigned int _maxLevel;
  bool _cacheTiles;
  double _splitDistance;
  MeshSize _meshSize;
  bool _useSkirts;
  SplitCallback::RefPtr _splitCallback;
  double _scale;

  SERIALIZE_XML_CLASS_NAME ( Body );
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BODY_CLASS_H_
