
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
#include "StarSystem/LandModel.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Jobs/Manager.h"

#include "boost/shared_ptr.hpp"

#include "osg/Vec2d"

namespace StarSystem { class Tile; class RasterLayer; class RasterGroup; }
namespace osg { class MatrixTransform; class Vec3f; class Texture; }


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

  // Helper macro for repeated code.
  STAR_SYSTEM_DEFINE_NODE_CLASS ( Body );

  // Constructors
  Body ( LandModel *, Usul::Jobs::Manager &, const MeshSize &, double splitDistance );

  // Add a tile for the given extents.
  void                      addTile ( const Extents & );

  // Set/get the flag that says to cache the tiles.
  bool                      cacheTiles() const;
  void                      cacheTiles ( bool );

  // Set/get the center.
  void                      center ( const Vec3d & );
  Vec3d                     center() const;

  // Get the elevation data.
  RasterLayer*              elevationData();

  // Append elevation data.
  void                      elevationAppend ( RasterLayer * );

  // Computes the "geodetic" radius for a given latitude in degrees.
  double                    geodeticRadius ( double latitude ) const;

  // Get the thread pool for this body.
  Usul::Jobs::Manager&      jobManager();

  // Convert lat, lon, height to x,y,z.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3f& point ) const;

  // Set/get the maximum level.
  void                      maxLevel ( unsigned int level );
  unsigned int              maxLevel() const;

  // Return the mesh size for the extents.
  MeshSize                  meshSize ( const Body::Extents &extents );

  // Pre- and post-render notifications.
  virtual void              preRender  ( Usul::Interfaces::IUnknown *caller );
  virtual void              postRender ( Usul::Interfaces::IUnknown *caller );

  // Append raster data.
  void                      rasterAppend ( RasterLayer * );

  // Get the scene.
  const osg::Node *         scene() const;
  osg::Node *               scene();

  // Request texture.
  int                       textureRequest ( const Extents &extents, unsigned int level );

  // Cancel request.
  void                      textureRequestCancel ( int id );

  // Get the texture.
  osg::Texture2D*           texture ( int id );

  // Set/get the flag that says to use skirts.
  void                      useSkirts ( bool );
  bool                      useSkirts() const;

protected:

  // Use reference counting.
  virtual ~Body();

private:

  typedef std::map < int, CutImageJob::RefPtr > TextureJobs;

  // No copying or assignment.
  Body ( const Body & );
  Body &operator = ( const Body & );

  void                      _destroy();

  osg::MatrixTransform *_transform;
  LandModel::RefPtr _landModel;
  RasterGroup *_rasters;
  RasterGroup *_elevation;
  Usul::Jobs::Manager &_manager;
  TextureJobs _textureJobs;
  bool _frame;
  unsigned int _texturesPerFrame;
  unsigned int _maxTexturesPerFrame;
  unsigned int _maxLevel;
  bool _cacheTiles;
  double _splitDistance;
  MeshSize _meshSize;
  bool _useSkirts;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_BODY_CLASS_H_
