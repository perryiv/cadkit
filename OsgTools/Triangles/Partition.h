
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_TRIANGLES_PARITION_H__
#define __OSG_TOOLS_TRIANGLES_PARITION_H__

#include "OsgTools/Export.h"
#include "OsgTools/Triangles/Triangle.h"

#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/BoundingBox"
#include "osg/Vec3"

namespace osg { class Node; }

namespace OsgTools {
namespace Triangles {

class SharedVertex;

class OSG_TOOLS_EXPORT Partition
{
public:
  Partition( );

  // Clear the partition.
  void clear();

  // Add a triangle
  void add ( Triangle *, const osg::Vec3Array &vertices, const osg::Vec3& n );

  // Get the vertex index of the i'th triangle in the drawable.
  unsigned int index0 ( const osg::Drawable *d, unsigned int i ) const;
  unsigned int index1 ( const osg::Drawable *d, unsigned int i ) const;
  unsigned int index2 ( const osg::Drawable *d, unsigned int i ) const;

  // Remove a triangle.
  void remove ( const osg::Drawable*, unsigned int index );

  // Reserve enough room.
  void reserve ( unsigned int );

  // Get the shared vertex of the i'th triangle in the drawable.
  SharedVertex* sharedVertex0 ( const osg::Drawable *d, unsigned int i );
  SharedVertex* sharedVertex1 ( const osg::Drawable *d, unsigned int i );
  SharedVertex* sharedVertex2 ( const osg::Drawable *d, unsigned int i );

  // Get the shared vertex of the i'th triangle in the drawable.
  const SharedVertex* sharedVertex0 ( const osg::Drawable *d, unsigned int i ) const;
  const SharedVertex* sharedVertex1 ( const osg::Drawable *d, unsigned int i ) const;
  const SharedVertex* sharedVertex2 ( const osg::Drawable *d, unsigned int i ) const;

  // Subdivide the partition.  Needs to be called before triangles are added.
  void subdivide ( const osg::BoundingBox&, unsigned int times );

  // Build the node.
  osg::Node* operator () ( osg::Vec3Array *vertices, osg::Vec3Array *normals, bool average );


private:
  
  // Get primitive set index for given vertices.
  unsigned int       _index ( const osg::Vec3& v0, const osg::Vec3& v1, const osg::Vec3& v2 );

  unsigned int       _indexn ( const osg::Drawable *d, unsigned int i, unsigned int n ) const;

  void               _divide ( const osg::BoundingBox &bb, osg::BoundingBox &bb1, osg::BoundingBox &bb2 );
  void               _subdivide ( const osg::BoundingBox&, unsigned int times );

  Triangle*          _triangle( const osg::Drawable *d, unsigned int i );
  const Triangle*    _triangle( const osg::Drawable *d, unsigned int i ) const;

  typedef std::vector < osg::BoundingBox > Cubes;
  typedef osg::DrawElementsUInt            DrawElements;
  typedef osg::ref_ptr < DrawElements >    PrimitiveSetPtr;
  typedef std::vector < PrimitiveSetPtr >  PrimitiveSets;
  typedef osg::ref_ptr< osg::Geometry >    GeometryPtr;
  typedef std::vector < GeometryPtr >      Geometries;
  typedef Triangle::ValidAccessRefPtr      TrianglePtr;
  typedef std::vector < TrianglePtr >      TriangleList;
  typedef std::vector < TriangleList >     Triangles;

  Cubes         _cubes;
  PrimitiveSets _primitiveSets;
  Geometries    _geometries;
  Triangles     _triangles;


}; // class Partition

} // namespace Triangles
} // namespace OsgTools


#endif // __OSG_TOOLS_TRIANGLES_PARITION_H__
