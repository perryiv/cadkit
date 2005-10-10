
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  A smart triangle set.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_
#define _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_

#ifdef _MSC_VER
#pragma warning ( disable : 4800 ) // Forcing value to bool 'true' or 'false'
#define WIN32_LEAN_AND_MEAN        // Exclude rarely-used stuff from Windows headers.
#define NOMINMAX                   // Do not define min and max as macros.
#endif

#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"
#include "OsgTools/Triangles/Factory.h"
#include "OsgTools/Triangles/Blocks.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/LessVector.h"

#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/BoundingBox"
#include "osg/Vec3f"

#ifndef __APPLE__
#include "boost/pool/pool_alloc.hpp"
#endif

#include <vector>
#include <map>
#include <string>

namespace osgUtil { class Hit; }
namespace osg { class Node; class Group; }

// Figure out what the problem is here... std::map::find fails but 
// std::map::insert returns existing element... why? This creates 
// problems because different osg::Vec3f end up becoming the same 
// SharedVertex, which makes a degenerate triangle.
#define USE_CLOSE_FLOAT_COMPARISON

namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT TriangleSet : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::pair < osg::Vec3f, SharedVertex::ValidAccessRefPtr > KeyValuePair;

  // Switch on/off the comparison predicate.
  #ifdef USE_CLOSE_FLOAT_COMPARISON
  typedef Usul::Predicates::CloseFloat < float > CloseFloat;
  typedef Usul::Predicates::LessVector < CloseFloat, 3 > LessVector;
  #else
  typedef std::less < KeyValuePair::first_type > LessVector;
  #endif

  // Mac doesn't like the boost memory pool for containers.
  #ifdef __APPLE__
  typedef std::map < KeyValuePair::first_type, KeyValuePair::second_type, LessVector > SharedVertices;
  #else 
  typedef boost::fast_pool_allocator < KeyValuePair > Allocator;
  typedef std::map < KeyValuePair::first_type, KeyValuePair::second_type, LessVector, Allocator > SharedVertices;
  #endif

  // Remaining typedefs.
  typedef std::vector < Triangle::ValidAccessRefPtr > TriangleVector;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map < std::string,std::string > Options;
  typedef osg::ref_ptr < osg::Vec3Array > VerticesPtr;
  typedef osg::ref_ptr < osg::Vec3Array > NormalsPtr;
  typedef osg::ref_ptr < osg::Vec4Array > ColorsPtr;
  typedef std::pair < NormalsPtr,NormalsPtr > Normals;
  typedef Blocks::ValidAccessRefPtr BlocksPtr;
  typedef std::vector < unsigned int > Indices;

  // Type information.
  USUL_DECLARE_TYPE_ID ( TriangleSet );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleSet );

  // Construction.
  TriangleSet();

  // Add a shared vertex.
  SharedVertex *          addSharedVertex ( const osg::Vec3f &v, bool look = true );

  // Add a triangle.
  void                    addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool update );
  void                    addTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, const osg::Vec3f &n, bool update );

  // Get the averaged normal for the shared vertex.
  osg::Vec3f              averageNormal ( const SharedVertex *sv ) const;

  // Build the scene
  osg::Node*              buildScene ( const Options &opt, Unknown *caller );

  // Perform exhaustive check of internal state.
  void                    checkStatus() const;

  // Clear existing data.
  void                    clear ( Unknown *caller = 0x0 );

  // Correct the normal vector.
  void                    correctNormal ( const Triangle *t, osg::Vec3f &normal ) const;
  void                    correctNormal ( const SharedVertex *sv0, const SharedVertex *sv1, const SharedVertex *sv2, osg::Vec3f &normal ) const;

  // Set/get dirty flags.
  void                    dirtyBlocks ( bool );
  bool                    dirtyBlocks() const;
  void                    dirtyNormalsV ( bool );
  bool                    dirtyNormalsV() const;

  // Get/Set the display list flag
  bool                    displayList () const;
  void                    displayList ( bool );

  // Is the triangle set empty?
  bool                    empty() const { return _triangles.empty(); }

  // Flip the normal vectors.
  void                    flipNormals();

  // Flip the normal of the i'th triangle
  void                    flipNormal( unsigned int );

  // Get the bounding box.
  osg::BoundingBox        getBoundingBox() const;

  // Get the vertex at the index
  const osg::Vec3f&       getVertex ( unsigned int index ) const;

  // Convert hit to triangle index.
  unsigned int            index ( const osgUtil::Hit &hit ) const;

  // Keep only these triangles
  void                    keep ( const std::vector < unsigned int>& keepers, Usul::Interfaces::IUnknown *caller );

  // Return a new shared vertex or triangle.
  SharedVertex *          newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0 );
  Triangle *              newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index );

  // Get the normal of the i'th triangle.
  const osg::Vec3f &      triangleNormal ( unsigned int ) const;

  // Access the containers of normals. Use with caution.
  const osg::Vec3Array *  normalsT()  const;
  osg::Vec3Array *        normalsT();
  const osg::Vec3Array *  normalsV() const;
  osg::Vec3Array *        normalsV();

  // Get the normal of the i'th vertex.
  const osg::Vec3f &      vertexNormal ( unsigned int ) const;

  // Remove these triangles
  void                    remove ( std::vector < unsigned int>& remove, Usul::Interfaces::IUnknown *caller );

  // Make space for the triangles.
  void                    reserve ( unsigned int );

  // Set all triangles and shared vertices to unvisited
  void                    setAllUnvisited();

  // Reset the on edge flag.
  void                    resetOnEdge();

  // Return the number of triangles.
  unsigned int            numTriangles() const { return _triangles.size(); }

  // Get the shared vertices. Be real careful when using this.
  const SharedVertices &  sharedVertices() const { return _shared; }
  SharedVertices &        sharedVertices()       { return _shared; }

  // Get the shared-vertices of the i'th triangle.
  const SharedVertex *    sharedVertex0 ( const osg::Drawable* d, unsigned int i ) const;
  const SharedVertex *    sharedVertex1 ( const osg::Drawable* d, unsigned int i ) const;
  const SharedVertex *    sharedVertex2 ( const osg::Drawable* d, unsigned int i ) const;

  // Get the triangles. Use with caution.
  const TriangleVector &  triangles() const { return _triangles; }
  TriangleVector &        triangles()       { return _triangles; }

  // Update the bounding box.
  void                    updateBounds ( Triangle *t );
  void                    updateBounds ( SharedVertex *v );
  void                    updateBounds ( const osg::Vec3f &v );

  // Get vertex pool. Use with caution.
  const osg::Vec3Array *  vertices() const;
  osg::Vec3Array *        vertices();

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &      vertex0 ( unsigned int ) const;
  const osg::Vec3f &      vertex1 ( unsigned int ) const;
  const osg::Vec3f &      vertex2 ( unsigned int ) const;

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &      vertex0 ( const osg::Drawable* d, unsigned int i ) const;
  const osg::Vec3f &      vertex1 ( const osg::Drawable* d, unsigned int i ) const;
  const osg::Vec3f &      vertex2 ( const osg::Drawable* d, unsigned int i ) const;

protected:

  // Do not copy.
  TriangleSet ( const TriangleSet & );
  TriangleSet &operator = ( const TriangleSet & );

  // Use reference counting.
  virtual ~TriangleSet();

  void                    _addToPartitionAndPerVertexNormals ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, Triangle *t, const osg::Vec3f &n );

  void                    _buildDecorations ( const Options &options, osg::Group * ) const;

  void                    _normalsT ( osg::Vec3Array *n );
  void                    _normalsV ( osg::Vec3Array *n );

  void                    _setProgressBar ( bool state, unsigned int numerator, unsigned int denominator );
  void                    _setStatusBar ( const std::string &text );

  void                    _updateBlocks();
  void                    _updateDependencies ( Triangle *t );
  void                    _updateNormalsV();
  void                    _updateNormalV ( SharedVertex *sv );

private:

  // Possible dirty flags.
  struct Dirty
  {
    enum
    {
      NORMALS_V = 0x00000001,
      BLOCKS    = 0x00000002,
    };
  };

  SharedVertices _shared;
  TriangleVector _triangles;
  VerticesPtr _vertices;
  Normals _normals;
  unsigned int _flags;
  osg::BoundingBox _bbox;
  Factory _factory;
  BlocksPtr _blocks;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_
