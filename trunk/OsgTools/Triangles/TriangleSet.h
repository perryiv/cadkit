
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
#include "Usul/Interfaces/IUnknown.h"

#include "OsgTools/Triangles/SharedVertex.h"
#include "OsgTools/Triangles/Triangle.h"

#include "osg/Geometry"
#include "osg/ref_ptr"

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
  typedef std::map < osg::Vec3f, SharedVertex::ValidAccessRefPtr > SharedVertices;
  typedef std::vector < Triangle::ValidAccessRefPtr > Triangles;

  // Type information.
  USUL_DECLARE_TYPE_ID ( TriangleSet );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleSet );

  // Construction.
  TriangleSet();

  // Delimit the start & finish of adding triangles.
  void                    addStart  ( Usul::Interfaces::IUnknown *caller );
  void                    addFinish ( Usul::Interfaces::IUnknown *caller );

  // Add a triangle.
  void                    addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n );

  // Clear existing data.
  void                    clear();

  // Flip the normal vectors.
  void                    flipNormals();

  // Get the normal of the i'th triangle.
  const osg::Vec3f &      normal ( unsigned int ) const;

  // Make space for the triangles.
  void                    reserve ( unsigned int );

  // Return the number of triangles.
  unsigned int            size() const { return _triangles.size(); }

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &      vertex0 ( unsigned int ) const;
  const osg::Vec3f &      vertex1 ( unsigned int ) const;
  const osg::Vec3f &      vertex2 ( unsigned int ) const;

protected:

  // Do not copy.
  TriangleSet ( const TriangleSet & );
  TriangleSet &operator = ( const TriangleSet & );

  // Use reference counting.
  virtual ~TriangleSet();

  const osg::Vec3Array &  _normalsPerFacet()  const { return *_normals.second; }
  osg::Vec3Array &        _normalsPerFacet()        { return *_normals.second; }
  const osg::Vec3Array &  _normalsPerVertex() const { return *_normals.first; }
  osg::Vec3Array &        _normalsPerVertex()       { return *_normals.first; }

  void                    _setProgressBar ( bool state, unsigned int numerator, unsigned int denominator, Unknown *caller );
  void                    _setStatusBar ( const std::string &text, Unknown *caller );

  SharedVertex *          _sharedVertex ( const osg::Vec3f &v );

private:

  typedef osg::ref_ptr<osg::Vec3Array> VerticesPtr;
  typedef osg::ref_ptr<osg::Vec3Array> NormalsPtr;
  typedef osg::ref_ptr<osg::Vec4Array> ColorsPtr;
  typedef std::pair<NormalsPtr,NormalsPtr> Normals;

  SharedVertices _shared;
  Triangles _triangles;
  VerticesPtr _vertices;
  Normals _normals;
  ColorsPtr _colors;
  bool _dirty;
  osg::ref_ptr<osg::Geometry> _geometry;
};


} // namespace Triangles
} // namespace OsgTools


#endif // _OPEN_SCENE_GRAPH_TOOLS_TRIANGLE_SET_CLASS_H_
