
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for objects that are represented by a sequence of vertices.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OBJECTS_VERTEX_SEQUENCE_OBJECT_H_
#define _OSG_TOOLS_OBJECTS_VERTEX_SEQUENCE_OBJECT_H_

#include "OsgTools/Objects/Object.h"

#include "Usul/Math/Vector4.h"

#include "osg/Geometry"
#include "osg/ref_ptr"

#include <vector>


namespace OsgTools {
namespace Objects {


class OSG_TOOLS_EXPORT VertexSequence : public OsgTools::Objects::Object
{
public:

  // Typedefs.
  typedef OsgTools::Objects::Object BaseClass;
  typedef osg::Vec3Array Vertices;
  typedef osg::Vec3Array Normals;
  typedef osg::Vec4Array Colors;
  typedef osg::Vec2Array TexCoords;
  typedef osg::ref_ptr<Vertices> VerticesPtr;
  typedef osg::ref_ptr<Normals> NormalsPtr;
  typedef osg::ref_ptr<Colors> ColorsPtr;
  typedef osg::ref_ptr<TexCoords> TexCoordsPtr;
  typedef osg::ref_ptr<osg::Geometry> GeometryPtr;

  // Boiler-plate code.
  OSG_TOOLS_OBJECTS_DECLARE_CLASS ( VertexSequence );

  // Get the geometry.
  GeometryPtr                         geometry();

  // Get the number.
  unsigned int                        numVertices()  const;
  unsigned int                        numNormals()   const;
  unsigned int                        numColors()    const;
  unsigned int                        numTexCoords() const;

  // Update the object.
  virtual void                        update ( Usul::Interfaces::IUnknown *caller );

protected:

  VertexSequence();
  VertexSequence ( const VertexSequence & );
  virtual ~VertexSequence();

  VerticesPtr                         _getVertices ( bool makeIfNeeded );
  NormalsPtr                          _getNormals ( bool makeIfNeeded );
  ColorsPtr                           _getColors ( bool makeIfNeeded );
  TexCoordsPtr                        _getTexCoords ( bool makeIfNeeded );

  void                                _setVertices  ( VerticesPtr );
  void                                _setNormals   ( NormalsPtr );
  void                                _setColors    ( ColorsPtr );
  void                                _setTexCoords ( TexCoordsPtr );

private:

  // No assignment.
  VertexSequence &operator = ( const VertexSequence & );

  void                                _destroy();

  GeometryPtr _geometry;
};


}; // namespace Objects
}; // namespace OsgTools


#endif // _OSG_TOOLS_OBJECTS_VERTEX_SEQUENCE_OBJECT_H_
