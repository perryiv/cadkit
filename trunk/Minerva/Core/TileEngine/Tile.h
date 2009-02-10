
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
//  A recursive tile.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_TILE_ENGINE_RECURSIVE_TILE_CLASS_H_
#define _MINERVA_CORE_TILE_ENGINE_RECURSIVE_TILE_CLASS_H_

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"
#include "Minerva/Core/Data/Container.h"
#include "Minerva/Core/TileEngine/Mesh.h"
#include "Minerva/Interfaces/IElevationData.h"
#include "Minerva/Interfaces/IIntersectNotify.h"
#include "Minerva/Interfaces/ITile.h"

#include "Usul/Base/Typed.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Interfaces/ITileVectorData.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Group"
#include "osg/Image"
#include "osg/observer_ptr"
#include "osg/Texture2D"
#include "osg/Vec3d"

#include "boost/shared_ptr.hpp"

namespace Usul { namespace Interfaces { struct IRasterLayer; } }
namespace Usul { namespace Jobs { class Manager; } }
namespace osgUtil { class CullVisitor; }
namespace Minerva { namespace Core { namespace TileEngine { class Body; } } }


namespace Minerva {
namespace Core {
namespace TileEngine {


class MINERVA_EXPORT Tile : 
  public osg::Group,
  public Minerva::Interfaces::ITile,
  public Minerva::Interfaces::IIntersectNotify
{
public:

  // OSG Plumbing.
  META_Node ( Minerva, Tile );

  // Declare smart pointers.
  USUL_DECLARE_REF_POINTERS ( Tile );

  // Type information.
  USUL_DECLARE_TYPE_ID ( Tile );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Indices for children
  enum Indices
  {
    NONE = -1,
    LOWER_LEFT = 0,
    LOWER_RIGHT = 1,
    UPPER_LEFT = 2,
    UPPER_RIGHT = 3
  };

  // Dirty flags.
  enum Flags
  {
    VERTICES   = 0x00000001,
    TEX_COORDS = 0x00000002,
    TEXTURE    = 0x00000004,
    CHILDREN   = 0x00000008,
    IMAGE      = 0x00000010,
    VECTOR     = 0x00000020,
    ALL        = VERTICES | TEX_COORDS | TEXTURE | VECTOR
  };

  // Useful typedefs.
  typedef osg::Group BaseClass;
  typedef osg::observer_ptr<Tile> WeakPtr;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Minerva::Core::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef MeshSize ImageSize;
  typedef std::vector < Tile::RefPtr > Tiles;
  typedef Tiles Children;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef osg::BoundingSphere BSphere;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef boost::shared_ptr<Mesh> MeshPtr;
  typedef std::pair<ImagePtr, Usul::Math::Vec4d> TextureData;
  typedef std::vector<TextureData> Textures;
  typedef Usul::Interfaces::IRasterLayer IRasterLayer;
  typedef std::pair<std::string, Usul::Math::Vec4d> ImageCacheData;
  typedef std::map <IRasterLayer::RefPtr, ImageCacheData> ImageCache;
  typedef Minerva::Core::Data::Container TileVectorData;
  typedef std::pair < TileVectorData::RefPtr, bool > TileVectorDataPair;
  typedef Usul::Interfaces::ITileVectorData::Jobs TileVectorJobs;
  typedef Minerva::Interfaces::IElevationData::QueryPtr ElevationDataPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef Minerva::Interfaces::IIntersectNotify IIntersectNotify;
  typedef IIntersectNotify::Closest Closest;

  // Constructors.
  Tile ( Tile* parent = 0x0,
         Indices index = Tile::NONE,
         unsigned int level = 0, 
         const Extents &extents = Extents ( Extents::Vertex ( 0, 0 ), Extents::Vertex ( 1, 1 ) ), 
         const MeshSize &meshSize = MeshSize ( 10, 10 ),
         const ImageSize& imageSize = ImageSize ( 512, 512 ),
         double splitDistance = 1,
         Body *body = 0x0,
         osg::Image * image = 0x0,
         ElevationDataPtr elevation = static_cast<Minerva::Interfaces::IElevationData*> ( 0x0 ),
         TileVectorData::RefPtr tileVectorData = 0x0 );
  Tile ( const Tile &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Add vector data.
  void                      addVectorData ( osg::Node* );

  // Clear the tile.
  void                      clear ( bool children );
  
  // Get the index of the child.
  Indices                   child ( Tile* ) const;

  // Get the child at index i.
  Tile::RefPtr              childAt ( unsigned int i ) const;

  // Build raster and vector data.
  void                      buildElevationData ( Usul::Jobs::Job::RefPtr );
  void                      buildPerTileVectorData ( Usul::Jobs::Job::RefPtr );
  void                      buildRaster ( Usul::Jobs::Job::RefPtr );

  // Compute the bounding sphere.
  virtual BSphere           computeBound() const;

  // Set the flag that says we're dirty.
  void                      dirty ( bool state, unsigned int flags, bool dirtyChildren );

  // Mark the dirty state if we cross this extents.
  void                      dirty ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents);

  // Get the individual dirty states.
  bool                      verticesDirty() const;
  bool                      texCoordsDirty() const;
  bool                      textureDirty() const;

  // Get the elevation at lat, lon.
  double                    elevation ( double lat, double lon );

  // Set/get the elevation data.
  void                      elevationData ( ElevationDataPtr data );
  ElevationDataPtr          elevationData() const;

  // Get the extents.
  Extents                   extents() const;

  // Call to notify of an intersection.
  virtual void              intersectNotify ( double x, double y, double z, double lon, double lat, double elev, IUnknown::RefPtr tile, IUnknown::RefPtr body, IUnknown::RefPtr caller, Closest & );

  // Get the image.
  ImagePtr                  image();

  // Is this tile a leaf?
  bool                      isLeaf() const;

  // Get the body's job manager.
  Usul::Jobs::Manager *     jobManager();

  // Convience function that re-directs to the body.
  void                      latLonHeightToXYZ ( double lat, double lon, double elevation, osg::Vec3d& point ) const;

  // Return level of this tile. Zero is the top.
  unsigned int              level() const;

  // Return the mutex. Use with caution.
  Mutex &                   mutex() const;

  // Get the size.
  MeshSize                  meshSize() const;

  // Remove vector data.
  void                      removeVectorData ( osg::Node* );

  // Get the texture coordinate range.
  Usul::Math::Vec4d         texCoords() const;

  // Get the image from the given RasterLayer.  May return null.
  TextureData               texture ( IRasterLayer * ) const;

  // Set the texture data.
  void                      textureData ( osg::Image* image, const Usul::Math::Vec4d& coords );

  // Minerva::Interfaces::ITile
  virtual Tile *            tile();

  // Traverse the children.
  virtual void              traverse ( osg::NodeVisitor & );
  
  // Get/Set the split distance.
  double                    splitDistance() const;
  void                      splitDistance( double distance, bool children );
  
  // Split the tile.
  void                      split ( Usul::Jobs::Job::RefPtr );
  
  // Update stuff.
  void                      updateAlpha();
  void                      updateMesh();
  void                      updateTexture();
  void                      updateTileVectorData();

protected:

  // Use reference counting.
  virtual ~Tile();

  // Cache the image used for the raster layer.
  void                      _cacheImage ( IRasterLayer::RefPtr raster, osg::Image* image, const Usul::Math::Vec4d& tCoords );

  void                      _cancelTileJob();
  void                      _cancelTileVectorJobs();

  void                      _cull ( osgUtil::CullVisitor &cv );

  /// Clear children.
  void                      _clearChildren ( bool traverse, bool cancelJob );

  void                      _deleteMe();
  
  // Load the image.
  void                      _launchImageRequest();
  void                      _launchElevationRequest();

  void                      _perTileVectorDataClear();
  void                      _perTileVectorDataDelete();
  bool                      _perTileVectorDataIsInherited() const;
  TileVectorData::RefPtr    _perTileVectorDataGet();

  // Quarter the texture coordinates.
  void                      _quarterTextureCoordinates ( Usul::Math::Vec4d& ll, Usul::Math::Vec4d& lr, Usul::Math::Vec4d& ul, Usul::Math::Vec4d& ur ) const;
  static Usul::Math::Vec4d  _textureCoordinatesSubRegion ( const Usul::Math::Vec4d& region, Indices index );

  // Build a tile.
  Tile::RefPtr              _buildTile ( unsigned int level, 
                                         const Extents& extents, 
                                         const MeshSize& size, 
                                         const Usul::Math::Vec4d& region, 
                                         double splitDistance, 
                                         Usul::Jobs::Job::RefPtr job,
                                         Indices index );
  
  // Build raster.
  static ImagePtr           _buildRaster ( const Extents &extents, unsigned int width, unsigned int height, unsigned int level, IRasterLayer* raster, Usul::Jobs::Job::RefPtr );
  
  void                      _setDirtyAlways ( bool state, unsigned int flags, bool dirtyChildren );
  void                      _setDirtyIfIntersect ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents );

  // Set the show border state.
  void                      _setShowBorders ( bool show );

  // Set the show skirt state.
  void                      _setShowSkirts ( bool show );

  // Update the border-showing state.
  void                      _updateShowBorders();

private:

  // No assignment.
  Tile &operator = ( const Tile & );

  void                      _destroy();
  
  // Set the image and texture coordinates.
  void                      _setImage ( osg::Image* );
  void                      _setTexCoords ( const Usul::Math::Vec4d& );

  mutable Mutex *_mutex;
  Body *_body;
  Extents _extents;
  double _splitDistance;
  MeshSize _meshSize;
  MeshPtr _mesh;
  unsigned int _level;
  unsigned int _flags;
  Children _children;
  ImagePtr _image;
  ElevationDataPtr _elevation;
  osg::ref_ptr < osg::Texture2D > _texture;
  Usul::Math::Vec4d _texCoords;
  Usul::Jobs::Job::RefPtr _imageJob;
  Usul::Jobs::Job::RefPtr _elevationJob;
  Usul::Jobs::Job::RefPtr _tileJob;
  osg::BoundingSphere _boundingSphere;
  osg::ref_ptr<osg::Group> _vector;
  ImageCache _textureMap;
  ImageSize _imageSize;
  WeakPtr _parent;
  Indices _index;
  TileVectorDataPair _tileVectorData;
  TileVectorJobs _tileVectorJobs;
};


} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_TILE_ENGINE_RECURSIVE_TILE_CLASS_H_
