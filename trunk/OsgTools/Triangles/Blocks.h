
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach & Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_ALL_PARITION_BLOCKS_H__
#define __OSG_TOOLS_TRIANGLES_ALL_PARITION_BLOCKS_H__

#include "OsgTools/Export.h"
#include "OsgTools/Triangles/Block.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Vector3.h"

#include "osg/BoundingBox"
#include "osg/Geode"
#include "osg/Material"
#include "osg/ref_ptr"


namespace OsgTools {
namespace Triangles {

class TriangleSet;

class OSG_TOOLS_EXPORT Blocks : public Usul::Base::Referenced
{
public:

  /// Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Block::Options Options;
  typedef std::vector < unsigned int > TriangleIndices;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Blocks );

  /// Construction
  Blocks ( const osg::BoundingBox &box, unsigned int numSubdivisions, unsigned int numTrianglesToReserve );

  /// Add a triangle.
  void                      addTriangle ( TriangleSet *ts, Triangle * );

  /// Get the block that contains the triangle or vertex.
  const Block *             block ( const osg::Vec3f & ) const;
  Block *                   block ( const osg::Vec3f & );
  const Block *             block ( const TriangleSet *ts, const Triangle * ) const;
  Block *                   block ( const TriangleSet *ts, const Triangle * );

  /// Return the bounding box.
  const osg::BoundingBox &  boundingBox() const { return _bbox; }

  /// Build the scene.
  osg::Geode *              buildScene ( const Options &options, TriangleSet *ts );

  /// Set the diffuse color.
  void                      colorDiffuse ( const osg::Vec4& );
  void                      colorSpecular ( const osg::Vec4& );

  /// Get/set display list state.
  bool                      displayList() const;
  void                      displayList ( bool );

  /// Get the geode for this blocks.
  const osg::Geode*         geode() const;
  osg::Geode*               geode();

  /// Purge any excess memory.
  void                      purge();

  /// Dirty the display lists.
  void                      setDirtyDisplayList();

  /// Get the indices of the triangles in the Blocks.
  void                      triangleIndices ( TriangleIndices& ) const;

protected:

  // Do not copy.
  Blocks ( const Blocks & );
  Blocks &operator = ( const Blocks & );

  // Use reference counting.
  virtual ~Blocks();

  void                      _indices ( const osg::Vec3f &, Usul::Math::Vec3ui & ) const;

  void                      _resize ( const Usul::Math::Vec3ui &size, unsigned int reserve );

  void                      _subdivide ( unsigned int times, unsigned int reserve );

private:

  typedef std::vector < Block::ValidRefPtr > Sequence;
  typedef std::vector < Sequence > Grid;
  typedef std::vector < Grid > Volume;
  typedef osg::ref_ptr < osg::Geode > GeodePtr;

  osg::BoundingBox _bbox;
  Volume _volume;
  Sequence _sequence;
  GeodePtr _geode;
  osg::ref_ptr< osg::Material > _material;
};


} // namespace Triangles
} // namespace OsgTools


#endif // __OSG_TOOLS_TRIANGLES_ALL_PARITION_BLOCKS_H__
