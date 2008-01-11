
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

#include "boost/shared_ptr.hpp"

#include "osg/Vec2d"
#include "osg/MatrixTransform"

namespace StarSystem { class Tile; }
namespace osg { class Vec3f; class Texture; }


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

  // Get the elevation data.
  RasterLayer::RefPtr       elevationData();

  // Append elevation data.
  void                      elevationAppend ( RasterLayer * );

  // Set/get the job manager for this body.
  void                      jobManager ( Usul::Jobs::Manager * );
  Usul::Jobs::Manager *     jobManager();

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;
  void                      xyzToLatLonHeight ( const osg::Vec3& point, double& lat, double& lon, double& elevation ) const;

  // Set/get the maximum level.
  void                      maxLevel ( unsigned int level );
  unsigned int              maxLevel() const;

  // Return the mesh size for the extents.
  MeshSize                  meshSize ( const Body::Extents &extents );

  // Pre- and post-render notifications.
  virtual void              preRender  ( Usul::Interfaces::IUnknown *caller );
  virtual void              postRender ( Usul::Interfaces::IUnknown *caller );

  // Append raster data.
  void                      rasterAppend ( Usul::Interfaces::IRasterLayer * );
  
  // Remove raster layer
  void                      rasterRemove ( Usul::Interfaces::IRasterLayer * );
  
  // Get the raster data.
  RasterLayer::RefPtr       rasterData();

  // Get the scale for the system.
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

  // Request texture.
  unsigned long             textureRequest ( const Extents &extents, unsigned int level );

  // Cancel request.
  void                      textureRequestCancel ( unsigned long id );

  // Get the texture.
  osg::Texture2D*           texture ( unsigned long id );

  // Set/get the flag that says to use skirts.
  void                      useSkirts ( bool );
  bool                      useSkirts() const;

protected:

  // Use reference counting.
  virtual ~Body();

private:

  typedef std::map < unsigned long, CutImageJob::RefPtr > TextureJobs;

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  MatrixTransformPtr _transform;
  LandModel::RefPtr _landModel;
  RasterGroup::RefPtr _rasters;
  ElevationGroup::RefPtr _elevation;
  Usul::Jobs::Manager *_manager;
  TextureJobs _textureJobs;
  bool _frame;
  unsigned int _texturesPerFrame;
  unsigned int _maxTexturesPerFrame;
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
