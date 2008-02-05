
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

#ifndef _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
#define _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_

#include "StarSystem/Export.h"
#include "StarSystem/Extents.h"
#include "StarSystem/CutImageJob.h"
#include "StarSystem/Mesh.h"

#include "Usul/Base/Typed.h"
#include "Usul/Interfaces/IRasterLayer.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Group"
#include "osg/Image"
#include "osg/Texture2D"
#include "osg/observer_ptr"
#include "osg/Vec3d"

#include <typeinfo>

namespace StarSystem { class RasterLayer; class Body; }
namespace osgUtil { class CullVisitor; }
class ossimEllipsoid;


namespace StarSystem {

class RasterLayer;

class STAR_SYSTEM_EXPORT Tile : public osg::Group
{
public:

  // OSG Plumbing.
  META_Node ( StarSystem, Tile );

  // Declare smart pointers.
  USUL_DECLARE_REF_POINTERS ( Tile );

  // Type information.
  USUL_DECLARE_TYPE_ID ( Tile );

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
    ALL        = VERTICES | TEX_COORDS | TEXTURE
  };

  // Useful typedefs.
  typedef osg::Group BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;
  typedef MeshSize ImageSize;
  typedef std::vector < Tile::RefPtr > Tiles;
  typedef Tiles Children;
  typedef std::pair < bool, unsigned long > JobID;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef osg::BoundingSphere BSphere;
  typedef osg::observer_ptr < Tile > WeakPtr;
  typedef osg::ref_ptr<osg::Node> NodePtr;
  typedef StarSystem::Mesh<osg::Vec3d> Mesh;
  typedef std::pair<ImagePtr, Usul::Math::Vec4d> TextureData;
  typedef std::vector<TextureData> Textures;
  typedef Usul::Interfaces::IRasterLayer IRasterLayer;
  typedef std::map <IRasterLayer::RefPtr, TextureData> TextureMap;

  // Constructors.
  Tile ( unsigned int level = 0, 
         const Extents &extents = Extents ( Extents::Vertex ( 0, 0 ), Extents::Vertex ( 1, 1 ) ), 
         const MeshSize &meshSize = MeshSize ( 10, 10 ),
         const ImageSize& imageSize = ImageSize ( 512, 512 ),
         double splitDistance = 1,
         Body *body = 0x0,
         osg::Image * image = 0x0,
         osg::Image * elevation = 0x0 );
  Tile ( const Tile &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Clear the scene.
  void                      clear();

  // Get the index of the child.
  Indices                   child ( Tile* ) const;
  
  // Build raster.
  static ImagePtr           buildRaster ( const Extents &extents, unsigned int width, unsigned int height, unsigned int level, RasterLayer* raster, Usul::Jobs::Job::RefPtr );

  // Build raster.
  void                      buildRaster ( const Usul::Math::Vec4d& region,
                                          Tile::RefPtr parent, 
                                          Usul::Jobs::Job::RefPtr );

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

  // Get the extents.
  Extents                   extents() const;

  // Get/Set the image.
  void                      image ( osg::Image* );
  ImagePtr                  image();

  // Return level of this tile. Zero is the top.
  unsigned int              level() const;

  // Return the mutex. Use with caution.
  Mutex &                   mutex() const;

  // Get the size.
  MeshSize                  meshSize() const;

  // Get/Set the starting texture coordinates.
  Usul::Math::Vec4d         texCoords() const;
  void                      texCoords ( const Usul::Math::Vec4d& );

  // Get the image from the given RasterLayer.  May return null.
  TextureData               texture ( IRasterLayer * ) const;

  // Set the texture data.
  void                      textureData ( osg::Image* image, const Usul::Math::Vec4d& coords );

  // Traverse the children.
  virtual void              traverse ( osg::NodeVisitor & );
  
  // Get the split distance.
  double                    splitDistance() const;
  
  // Split the tile.
  void                      split ( Usul::Jobs::Job::RefPtr );
  
  // Update mesh.
  void                      updateMesh();
  
  // Update texture.
  void                      updateTexture();

protected:

  // Use reference counting.
  virtual ~Tile();

  NodePtr                   _buildBorderLine();

  // Build skirts.
  osg::Node*                _buildLonSkirt ( double lon, double u, unsigned int i, double offset, const Mesh::Vector& ll );
  osg::Node*                _buildLatSkirt ( double lat, double v, unsigned int j, double offset, const Mesh::Vector& ll );

  void                      _cull ( osgUtil::CullVisitor &cv );

  /// Clear children.
  void                      _clearChildren();

  void                      _deleteMe();

  // Load the image.
  void                      _launchImageRequest();
  void                      _launchElevationRequest();

  // Quarter the texture coordinates.
  void                      _quarterTextureCoordinates ( Usul::Math::Vec4d& ll, Usul::Math::Vec4d& lr, Usul::Math::Vec4d& ul, Usul::Math::Vec4d& ur ) const;
  static Usul::Math::Vec4d  _textureCoordinatesSubRegion ( const Usul::Math::Vec4d& region, Indices index );

  // Build a tile.
  Tile::RefPtr              _buildTile ( unsigned int level, 
                                         const Extents& extents, 
                                         const MeshSize& size, 
                                         const Usul::Math::Vec4d& region, 
                                         double splitDistance, 
                                         Usul::Jobs::Job::RefPtr job );


  void                      _setDirtyAlways ( bool state, unsigned int flags, bool dirtyChildren );
  void                      _setDirtyIfIntersect ( bool state, unsigned int flags, bool dirtyChildren, const Extents& extents );

private:

  // No assignment.
  Tile &operator = ( const Tile & );

  void                      _destroy();

  mutable Mutex *_mutex;
  Body *_body;
  Extents _extents;
  double _splitDistance;
  Mesh *_mesh;
  osg::Vec3d _lowerLeft;
  unsigned int _level;
  unsigned int _flags;
  Children _children;
  unsigned int _textureUnit;
  osg::ref_ptr < osg::Image > _image;
  osg::ref_ptr < osg::Image > _elevation;
  osg::ref_ptr < osg::Texture2D > _texture;
  Usul::Math::Vec4d _texCoords;
  CutImageJob::RefPtr _imageJob;
  CutImageJob::RefPtr _elevationJob;
  Usul::Jobs::Job::RefPtr _tileJob;
  osg::BoundingSphere _boundingSphere;
  osg::ref_ptr<osg::Group> _borders;
  TextureMap _textureMap;
  ImageSize _imageSize;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
