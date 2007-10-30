
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

#include "Usul/Base/Typed.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

#include "osg/Group"

#include <typeinfo>

namespace StarSystem { class RasterLayer; class Body; }
namespace OsgTools { class Mesh; }
namespace osgUtil { class CullVisitor; }
class ossimEllipsoid;


namespace StarSystem {


class STAR_SYSTEM_EXPORT Tile : public osg::Group
{
public:

  // OSG Plumbing.
  META_Node ( osg, Tile );

  // Declare smart pointers.
  USUL_DECLARE_REF_POINTERS ( Tile );

  // Type information.
  USUL_DECLARE_TYPE_ID ( Tile );

  // Indices for children
  enum Indices
  {
    LOWER_LEFT,
    LOWER_RIGHT,
    UPPER_LEFT,
    UPPER_RIGHT
  };

  // Useful typedefs.
  typedef osg::Group BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard<Mutex> Guard;
  typedef Usul::Math::Vector4 < Tile::RefPtr > Children;
  typedef StarSystem::Extents < osg::Vec2d > Extents;
  typedef Usul::Math::Vec2ui MeshSize;

  // Constructors.
  Tile ( unsigned int level = 0, 
         const Extents &extents = Extents ( Extents::Vertex ( 0, 0 ), Extents::Vertex ( 1, 1 ) ), 
         const MeshSize &meshSize = MeshSize ( 10, 10 ),
         double splitDistance = 1,
         Body *body = 0x0,
         RasterLayer *raster = 0x0 );
  Tile ( const Tile &, const osg::CopyOp &copyop = osg::CopyOp::SHALLOW_COPY );

  // Set/get the flag that says we're dirty.
  bool                      dirty() const;
  void                      dirty ( bool state, bool dirtyChildren );

  // Mark the dirty state, only if we cross this extents.
  void                      dirty ( bool state, bool dirtyChildren, const Extents& extents);

  // Return level of this tile. Zero is the top.
  unsigned int              level() const;

  // Return the mutex. Use with caution.
  Mutex &                   mutex() const;

  // Traverse the children.
  virtual void              traverse ( osg::NodeVisitor & );

protected:

  // Use reference counting.
  virtual ~Tile();

  void                      _cull ( osgUtil::CullVisitor &cv );

  void                      _update();
  
  // Build skirts.
  osg::Node*                _buildLonSkirt ( double lon, double u, double offset );
  osg::Node*                _buildLatSkirt ( double lat, double v, double offset );

private:

  // No assignment.
  Tile &operator = ( const Tile & );

  void                      _destroy();

  mutable Mutex *_mutex;
  Body *_body;
  Extents _extents;
  double _splitDistance;
  OsgTools::Mesh *_mesh;
  unsigned int _level;
  bool _dirty;
  RasterLayer *_raster;
  Children _children;
  unsigned int _textureUnit;
};


} // namespace StarSystem


#endif // _STAR_SYSTEM_RECURSIVE_TILE_CLASS_H_
