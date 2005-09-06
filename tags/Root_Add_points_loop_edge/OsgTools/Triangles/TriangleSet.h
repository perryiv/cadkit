
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

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Predicates/LessVector.h"
#include "Usul/Interfaces/IUnknown.h"

#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"
#include "OsgTools/Triangles/Partition.h"

#include "osg/Geometry"
#include "osg/ref_ptr"
#include "osg/BoundingBox"
#include "osg/Vec3f"

namespace osg { class Node; }

#include <vector>
#include <map>
#include <string>


namespace OsgTools {
namespace Triangles {


class OSG_TOOLS_EXPORT TriangleSet : public Usul::Base::Referenced
{
public:

  // Useful typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Predicates::CloseFloat < float > CloseFloat;
  typedef Usul::Predicates::LessVector < CloseFloat, 3 > LessVector;
  typedef std::map < osg::Vec3f, SharedVertex::ValidAccessRefPtr, LessVector > SharedVertices;
  typedef std::vector < Triangle::ValidAccessRefPtr > Triangles;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::map<std::string,std::string> Options;

  // Type information.
  USUL_DECLARE_TYPE_ID ( TriangleSet );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleSet );

  // Construction.
  TriangleSet();

  // Delimit the start & finish of adding triangles.
  void                    addStart  ( );
  void                    addFinish ( );

  // Add a shared vertex.  Is not added to the map.
  SharedVertex*           addSharedVertex ( const osg::Vec3f& );

  // Add a triangle.
  void                    addTriangle ( const osg::Vec3f &v0, 
                                        const osg::Vec3f &v1, 
                                        const osg::Vec3f &v2, 
                                        const osg::Vec3f &n, 
                                        bool correctNormal = false, 
                                        bool rebuild = false );

  void                    addTriangle ( const SharedVertex &v0, 
                                        const SharedVertex &v1, 
                                        const SharedVertex &v2, 
                                        const osg::Vec3f &n, 
                                        bool correctNormal = false, 
                                        bool rebuild = false );

  /// Build the scene
  osg::Node*              buildScene ( const Options &opt, Unknown *caller );

  // Clear existing data.
  void                    clear ( Unknown *caller = 0x0 );

  // Turn on color and set all trianlges to given color
  void                    colorOn ( const osg::Vec4& color );

  // Color the ith triangle with given color
  void                    color ( unsigned int index, const osg::Vec4& color );

  //Turn off color
  void                    colorOff ();

  // Delete triangle at given index
  void                    deleteTriangle ( const osg::Drawable*, unsigned int index );

  // Get/Set the display list flag
  bool                    displayList () const;
  void                    displayList ( bool );

  // Flip the normal vectors.
  void                    flipNormals();

  // Flip the normal of the i'th triangle
  void                    flipNormal( unsigned int );

  // Get the bounding box.
  const osg::BoundingBox& getBoundingBox () const;

  // Get the vertex at the index
  const osg::Vec3f&       getVertex ( unsigned int index ) const;

  // Keep only these triangles
  void                    keep ( const std::vector<unsigned int>& keepers, Usul::Interfaces::IUnknown *caller );

  // Get the normal of the i'th triangle.
  const osg::Vec3f &      normal ( unsigned int ) const;

  // Get the normal of the i'th vertex.
  const osg::Vec3f &      vertexNormal ( unsigned int ) const;

  // Remove these triangles
  void                    remove ( std::vector<unsigned int>& remove, Usul::Interfaces::IUnknown *caller );

  // Make space for the triangles.
  void                    reserve ( unsigned int );

  // Set all triangles and shared vertices to unvisited
  void                    setAllUnvisited();

  // Reset the on edge flag.
  void                    resetOnEdge();

  // Return the number of triangles.
  unsigned int            size() const { return _triangles.size(); }

  // Get the shared vertices of the i'th triangle in the drawable.
  const SharedVertex*     sharedVertex0 ( const osg::Drawable*, unsigned int i ) const;
  const SharedVertex*     sharedVertex1 ( const osg::Drawable*, unsigned int i ) const;
  const SharedVertex*     sharedVertex2 ( const osg::Drawable*, unsigned int i ) const;

  // Get the triangles
  const Triangles&        triangles() const { return _triangles; }
  Triangles&              triangles()       { return _triangles; }

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

  void                    _addTriangle ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, const osg::Vec3f &n, bool correctNormal, bool rebuild );

  void                    _setMaxMinValues(SharedVertex *sv0);
  
  //Get the averaged normal for the shared vertex
  osg::Vec3               _averageNormal ( const SharedVertex *sv ) const;

  const osg::Vec3Array &  _normalsPerFacet()  const { return *_normals.second; }
  osg::Vec3Array &        _normalsPerFacet()        { return *_normals.second; }
  const osg::Vec3Array &  _normalsPerVertex() const { return *_normals.first; }
  osg::Vec3Array &        _normalsPerVertex()       { return *_normals.first; }

  void                    _setProgressBar ( bool state, unsigned int numerator, unsigned int denominator );
  void                    _setStatusBar ( const std::string &text );

  SharedVertex *          _sharedVertex ( const osg::Vec3f &v );

private:

  typedef osg::ref_ptr<osg::Vec3Array>        VerticesPtr;
  typedef osg::ref_ptr<osg::Vec3Array>        NormalsPtr;
  typedef osg::ref_ptr<osg::Vec4Array>        ColorsPtr;
  typedef std::pair<NormalsPtr,NormalsPtr>    Normals;

  SharedVertices _shared;
  Triangles _triangles;
  VerticesPtr _vertices;
  Normals _normals;
  ColorsPtr _colors;
  bool _dirty;
  osg::BoundingBox _bb;
  Partition _partition;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_
