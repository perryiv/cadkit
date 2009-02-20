
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
// #define NOMINMAX                   // Do not define min and max as macros.
#endif

#include "OsgTools/Configure/OSG.h"
#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"
#include "OsgTools/Triangles/Factory.h"
#include "OsgTools/Triangles/Blocks.h"
#include "OsgTools/Triangles/ColorFunctor.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/ILoadColorFile.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/LessVector.h"
#include "Usul/Types/Types.h"

#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/BoundingBox"
#include "osg/Vec3f"
#include "osg/Group"

#include "osgUtil/LineSegmentIntersector"

#include <vector>
#include <map>
#include <string>


namespace osgUtil { class Hit; }
namespace osg { class Node; class Group; }


namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT TriangleSet : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::pair < osg::Vec3f, SharedVertex::ValidAccessRefPtr > KeyValuePair;

  // The comparison predicate.
  typedef Usul::Predicates::CloseFloat < float > CloseFloat;
  typedef Usul::Predicates::LessVector < CloseFloat, 3 > LessVector;

  // Remaining typedefs.
  typedef std::map < KeyValuePair::first_type, KeyValuePair::second_type, LessVector > SharedVertices;
  typedef std::vector < Triangle::ValidAccessRefPtr > TriangleVector;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map < std::string,std::string > Options;
  typedef osg::ref_ptr < osg::Vec3Array > VerticesPtr;
  typedef osg::ref_ptr < osg::Vec3Array > NormalsPtr;
  typedef osg::ref_ptr < osg::Vec4Array > ColorsPtr;
  typedef Blocks::ValidAccessRefPtr BlocksPtr;
  typedef std::vector < BlocksPtr > BlocksVector;
  typedef BlocksVector::iterator    BlocksIterator;
  typedef BlocksVector::const_iterator    BlocksConstIterator;
  typedef std::vector < unsigned int > Indices;
  typedef std::pair < unsigned int, unsigned int > Progress;
  typedef std::pair < SharedVertices::iterator, bool > InsertResult;
  typedef Factory::ValidRefPtr FactoryPtr;
  typedef std::vector< unsigned int > Connected;
  typedef std::vector < Connected > Subsets;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  //typedef ILoadColorFile::HeaderInfo HeaderInfo;
  typedef std::vector< float > HeaderInfo;

  // Type information.
  USUL_DECLARE_TYPE_ID ( TriangleSet );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleSet );

  // Construction. Pass the number of adjacent floating point numbers that are the "same".
  TriangleSet ( unsigned int unitsInLastPlace = 1 );

  // Add a shared vertex. Pass false for "look" if you know the vertex is unique.
  SharedVertex *          addSharedVertex ( const osg::Vec3f &v, bool look );

  // Add a triangle. Pass false for "look" if you know the vertex is unique.
  Triangle *              addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool update, bool look );
  Triangle *              addTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, const osg::Vec3f &n, bool update );

  // Get the averaged normal for the shared vertex.
  osg::Vec3f              averageNormal ( const SharedVertex *sv ) const;

  /// Get the number of blocks.
  unsigned int            blocksNumber() const;

  /// Get the blocks as an IUknown
  Unknown*                blocks ( unsigned int i );

  /// Iterator to the begining of the blocks.
  BlocksIterator          blocksBegin()        { return _blocks.begin(); }
  BlocksConstIterator     blocksBegin() const  { return _blocks.begin(); }

  /// Iterator to the end of the blocks.
  BlocksIterator          blocksEnd()        { return _blocks.end(); }
  BlocksConstIterator     blocksEnd() const  { return _blocks.end(); }

  // Build the scene
  osg::Node*              buildScene ( const Options &options, Unknown *caller );

  // Perform exhaustive check of internal state.
  void                    checkStatus() const;

  // Clear existing data.
  void                    clear ( Unknown *caller = 0x0 );

  // Return the color.
  osg::Vec4f              color ( const Triangle *t ) const;
  osg::Vec4f              color ( const SharedVertex *t ) const;

  // Set/get the color functor.
  void                    colorFunctor ( ColorFunctor * );
  ColorFunctor *          colorFunctor() const;

  // Access the container of colors. Use with caution.
  const osg::Vec4Array *  colorsV()  const;
  osg::Vec4Array *        colorsV();

  // Correct the normal vector.
  void                    correctNormal ( const Triangle *t, osg::Vec3f &normal ) const;
  void                    correctNormal ( const SharedVertex *sv0, const SharedVertex *sv1, const SharedVertex *sv2, osg::Vec3f &normal ) const;

  // Create subsets.
  void                    createSubsets ( const Subsets& subsets, Unknown *caller );

  // Set/get dirty flags.
  void                    dirtyBlocks ( bool );
  bool                    dirtyBlocks() const;
  void                    dirtyColorsV ( bool );
  bool                    dirtyColorsV() const;
  void                    dirtyNormalsV ( bool );
  bool                    dirtyNormalsV() const;

  // Get/Set the display list flag.
  bool                    displayList () const;
  void                    displayList ( bool );
  void                    setDirtyDisplayList();

  // Is the triangle set empty?
  bool                    empty() const { return _triangles.empty(); }

  // Access to the factory. Use with caution.
  const Factory *         factory() const { return _factory; }
  Factory *               factory()       { return _factory; }

  /// Find all triangles connected to seed and place them in connected
  void                    findAllConnected ( Usul::Interfaces::IUnknown* caller, Connected& connected, unsigned int seed, bool showProgress, bool clearFlags );

  // Flip the normal vectors.
  void                    flipNormals();

  // Flip the normal of the i'th triangle.
  void                    flipNormal( unsigned int );

  // Get the bounding box.
  osg::BoundingBox        getBoundingBox() const;

  // Get the color values per vertex
  osg::Vec4Array *        getColorsV ( bool reserve );

  // Get the vertex at the index.
  const osg::Vec3f&       getVertex ( unsigned int index ) const;

  /// Group the triangles.
  void                    groupTriangles ( Usul::Interfaces::IUnknown *caller );

  // Convert hit to triangle index.
  unsigned int            index ( const osgUtil::LineSegmentIntersector::Intersection &hit ) const;

  // Keep only these triangles.
  void                    keepTriangles ( const Indices &keepers, Usul::Interfaces::IUnknown *caller );

  // Return a new shared vertex or triangle.
  SharedVertex *          newSharedVertex ( unsigned int index, unsigned int numTrianglesToReserve = 0 );
  Triangle *              newTriangle ( SharedVertex *v0, SharedVertex *v1, SharedVertex *v2, unsigned int index );

  // Get the number of subdivisions.
  unsigned int            numberSubDivisions( unsigned int numberTriangles );

  // Get the triangle given a drawable and primitive index.
  const Triangle *        triangle ( const osg::Geode* g, const osg::Drawable *d, unsigned int num ) const;
  Triangle *              triangle ( const osg::Geode* g, const osg::Drawable *d, unsigned int num );

  // Get the center of the triangle.
  osg::Vec3f              triangleCenter ( unsigned int ) const;

  // Get the normal of the i'th triangle.
  const osg::Vec3f &      triangleNormal ( unsigned int ) const;

  // Access the containers of normals. Use with caution.
  const osg::Vec3Array *  normalsT()  const;
  osg::Vec3Array *        normalsT();
  const osg::Vec3Array *  normalsV() const;
  osg::Vec3Array *        normalsV();

  // Return the number of triangles.
  unsigned int            numTriangles() const { return _triangles.size(); }

  // Purge memory.
  void                    purge();

  // Remove these triangles
  void                    removeTriangles ( Indices &doomed, Usul::Interfaces::IUnknown *caller );
  void                    removeTriangle ( const osg::Geode* g, const osg::Drawable *d, unsigned int num );

  // Make space for the triangles.
  void                    reserve ( unsigned int );

  // Reset the on edge flag.
  void                    resetOnEdge();

  // Set all triangles and shared vertices to unvisited
  void                    setAllUnvisited();

  // Sets triangles as visited based on a vector of indices
  void                    setVisited(Indices &keepers);
  
  // Returns the index of the first triangle flagged as visited=FALSE
  Usul::Types::Int32      firstUnvisited();

  // Get the shared vertices. Be real careful when using this.
  const SharedVertices &  sharedVertices() const { return _shared; }
  SharedVertices &        sharedVertices()       { return _shared; }

  // Get the shared-vertices of the i'th triangle.
  const SharedVertex *    sharedVertex0 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;
  const SharedVertex *    sharedVertex1 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;
  const SharedVertex *    sharedVertex2 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;
  SharedVertex *          sharedVertex0 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i );
  SharedVertex *          sharedVertex1 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i );
  SharedVertex *          sharedVertex2 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i );

  /// Get a node that shows the new triangles.
  osg::Node*              showNewTriangles();

  // Get the triangles. Use with caution.
  const TriangleVector &  triangles() const { return _triangles; }
  TriangleVector &        triangles()       { return _triangles; }

  // Update the bounding box.
  void                    updateBounds ( Triangle *t );
  void                    updateBounds ( SharedVertex *v );
  void                    updateBounds ( const osg::Vec3f &v );

  /// Set the use material flag.
  void                    useMaterial ( bool b );

  // Get vertex pool. Use with caution.
  const osg::Vec3Array *  vertices() const;
  osg::Vec3Array *        vertices();

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &      vertex0 ( unsigned int ) const;
  const osg::Vec3f &      vertex1 ( unsigned int ) const;
  const osg::Vec3f &      vertex2 ( unsigned int ) const;

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &      vertex0 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;
  const osg::Vec3f &      vertex1 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;
  const osg::Vec3f &      vertex2 ( const osg::Geode* g, const osg::Drawable* d, unsigned int i ) const;

  // Get the normal of the i'th vertex.
  const osg::Vec3f &      vertexNormal ( unsigned int ) const;

  void                    loadColorFile( const std::string &filename, const HeaderInfo& header );

protected:

  // Do not copy.
  TriangleSet ( const TriangleSet & );
  TriangleSet &operator = ( const TriangleSet & );

  // Use reference counting.
  virtual ~TriangleSet();

  Blocks*                 _blocksGet ( const osg::Geode* g ) const;

  void                    _buildDecorations ( const Options &options, osg::Group * ) const;

  void                    _incrementProgress ( bool state );
  InsertResult            _insertSharedVertex ( const osg::Vec3f &v, SharedVertex *sv );

  void                    _setProgressBar ( bool state, unsigned int numerator, unsigned int denominator );
  void                    _setStatusBar ( const std::string &text );

  void                    _updateBlocks();
  void                    _updateDependencies ( Triangle *t );
  void                    _updateColorsV();
  void                    _updateNormalsV();
  void                    _updateColorV  ( SharedVertex *sv );
  void                    _updateNormalV ( SharedVertex *sv );

  
private:

  // Possible dirty flags.
  struct Dirty
  {
    enum DirtyFlags
    {
      NORMALS_V = 0x00000001,
      COLORS_V  = 0x00000002,
      BLOCKS    = 0x00000004
    };
  };

  SharedVertices _shared;
  TriangleVector _triangles;
  VerticesPtr _vertices;
  NormalsPtr _normalsV;
  NormalsPtr _normalsT;
  ColorsPtr _colorsV;
  unsigned int _flags;
  osg::BoundingBox _bbox;
  FactoryPtr _factory;
  BlocksVector _blocks;
  Progress _progress;
  ColorFunctor::RefPtr _color;
  GroupPtr _root;
  bool _useMaterial;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_
