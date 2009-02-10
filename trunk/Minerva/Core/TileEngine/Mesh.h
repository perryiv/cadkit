
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

#include "Minerva/Interfaces/IElevationData.h"

#include "OsgTools/Configure/OSG.h"

#include "osg/BoundingSphere"
#include "osg/Geometry"
#include "osg/Image"
#include "osg/Vec2d"
#include "osg/Vec3d"

#include <vector>

namespace osg { class Geode; class Group; class Node; }

namespace Minerva {
namespace Core {
namespace TileEngine {

  class Body;
  class LandModel;

class MINERVA_EXPORT Mesh
{
public:

  // Useful typedefs.
  typedef osg::Vec3d Vector;
  typedef Vector Vertex;
  typedef std::vector<Vector> Vectors;
  typedef Vectors::reference reference;
  typedef Vectors::const_reference const_reference;
  typedef Vectors::value_type value_type;
  typedef Vectors::size_type size_type;
  typedef osg::ref_ptr<osg::Image> ImagePtr;
  typedef Minerva::Core::Extents<osg::Vec2d> Extents;
  typedef Minerva::Interfaces::IElevationData::RefPtr ElevationDataPtr;

  Mesh ( unsigned int rows, unsigned int columns, double skirtHeight, const Extents& extents );

  // The number of columns.
  unsigned int        columns() const { return _columns; }

  // Build the mesh.
  osg::Node*          buildMesh ( const Body&, 
                                  ElevationDataPtr elevation, 
                                  const osg::Vec2d& uRange,
                                  const osg::Vec2d& vRange,
                                  osg::BoundingSphere& boundingSphere );

  // Get the smallest distance (squared) from the given point.
  double              getSmallestDistanceSquared ( const osg::Vec3d& point ) const;

  // Get the elevation value from the triangles at a given lat,lon.
  double              elevation ( double lat, double lon, const LandModel& land ) const;

  // The number of rows.
  unsigned int        rows() const { return _rows; }

  // Show the border.
  void                showBorder ( bool b );

  // Show the skirts.
  void                showSkirts ( bool b );

private:

  // Build line-segment for the border.
  osg::Node*          _buildBorder() const;

  // Build the geometries for the mesh and skirts.
  void                _buildGeometry ( osg::Geode& mesh, osg::Geode& skirts ) const;

  // Is the point contained in the triangle?
  static bool         _containsPoint ( const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& point, Vertex& weights );

  // Get the index for the row and column.
  inline size_type    _index ( size_type row, size_type column ) const { return row * _columns + column; }
  
  // Access to a single point.
  const_reference     _point ( size_type row, size_type column ) const;

  // Set the location data.
  void                _setLocationData ( const Body& body, osg::BoundingSphere& boundingSphere, unsigned int i, unsigned int j, double lat, double lon, double elevation, double s, double t );

  // Useful typedefs.
  typedef osg::Vec2d TexCoord;
  typedef std::vector<TexCoord> TexCoords;
  typedef unsigned short IndexType;
  typedef std::vector<IndexType> Indices;
  typedef std::vector<Indices> Primitives;
  typedef std::vector<Vertex> LatLonPoints;

  Mesh();
  Mesh ( const Mesh & );
  Mesh &operator = ( const Mesh & );

  LatLonPoints _latLonPoints;
  Vectors _points;
  Vectors _normals;
  TexCoords _texCoords;
  Primitives _meshPrimitives;
  unsigned int _rows;
  unsigned int _columns;
  double _skirtHeight;
  Extents _extents;

  osg::ref_ptr<osg::Group> _borders;
  osg::ref_ptr<osg::Node>  _skirts;
  osg::ref_ptr<osg::Node>  _ground;

  osg::Vec3d _lowerLeft;
  osg::BoundingSphere _boundingSphere;
};

  
} // namespace TileEngine
} // namespace Core
} // namespace Minerva


#endif // _MINERVA_CORE_SURFACE_MESH_BUILDER_H_
