
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a surface mesh.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MINERVA_CORE_SURFACE_MESH_BUILDER_H_
#define _MINERVA_CORE_SURFACE_MESH_BUILDER_H_

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Extents.h"

#include "OsgTools/Configure/OSG.h"

#include "osg/Image"
#include "osg/Vec2d"
#include "osg/Vec3d"

#include <vector>

namespace osg { class Geode; class BoundingSphere; }

namespace Minerva {
namespace Core {
namespace TileEngine {

  class Body;

class MINERVA_EXPORT Mesh
{
public:

  // Useful typedefs.
  typedef osg::Vec3d Vector;
  typedef std::vector<Vector> Vectors;
  typedef Vectors::reference reference;
  typedef Vectors::const_reference const_reference;
  typedef Vectors::value_type value_type;
  typedef Vectors::size_type size_type;
  typedef osg::Vec2d TexCoord;
  typedef std::vector < TexCoord > TexCoords;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;

  Mesh ( unsigned int rows, unsigned int columns, double skirtHeight );

  // The number of columns.
  unsigned int        columns() const { return _columns; }

  // Build the mesh.
  void                buildMesh ( const Body&, 
                                  const Extents& extents, 
                                  ImagePtr elevation, 
                                  const osg::Vec2d& uRange,
                                  const osg::Vec2d& vRange,
                                  osg::BoundingSphere& boundingSphere,
                                  Vector& offset );

  // Access to a single point.
  const_reference     point ( size_type row, size_type column ) const;

  // The number of rows.
  unsigned int        rows() const { return _rows; }

  // Generate a scene.
  void                operator() ( osg::Geode& mesh, osg::Geode& skirts ) const;

private:
  
  // Set the location data.
  void                _setLocationData ( const Body& body, osg::BoundingSphere& boundingSphere, unsigned int i, unsigned int j, double lat, double lon, double elevation, double s, double t );

  Mesh();
  Mesh ( const Mesh & );
  Mesh &operator = ( const Mesh & );
  
  Vectors _points;
  Vectors _normals;
  TexCoords _texCoords;
  unsigned int _rows;
  unsigned int _columns;
  double _skirtHeight;
};

  
} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_SURFACE_MESH_BUILDER_H_
