
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

#ifndef _OSG_TOOLS_SURFACE_MESH_BUILDER_H_
#define _OSG_TOOLS_SURFACE_MESH_BUILDER_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"

#include "osg/Vec3"

#include <vector>


namespace OsgTools {


class OSG_TOOLS_EXPORT Mesh
{
public:

  // Useful typedefs.
  typedef osg::Vec3 Vector;
  typedef std::vector<Vector> Vectors;
  typedef Vectors::reference reference;
  typedef Vectors::const_reference const_reference;
  typedef Vectors::value_type value_type;
  typedef Vectors::size_type size_type;

  Mesh();
  Mesh ( unsigned int rows, unsigned int columns );
  Mesh ( const Mesh & );
  Mesh &operator = ( const Mesh & );

  // Allocate the mesh.
  void                allocatePoints();
  void                allocateNormals();

  // The number of columns.
  unsigned int        columns() const { return _columns; }

  // Normalize all the normal vectors (make them have a length of one).
  void                normalize();

  // Access to the normals.
  void                normals ( const Vectors &n ) { _normals = n; }
  Vectors &           normals()       { return _normals; }
  const Vectors &     normals() const { return _normals; }

  // Access to a single normal.
  reference           normal ( size_type row, size_type column );
  const_reference     normal ( size_type row, size_type column ) const;

  // Access to the points.
  void                points ( const Vectors &p ) { _points = p; }
  Vectors &           points()       { return _points; }
  const Vectors &     points() const { return _points; }

  // Access to a single point.
  reference           point ( size_type row, size_type column );
  const_reference     point ( size_type row, size_type column ) const;

  // The number of rows.
  unsigned int        rows() const { return _rows; }

  // Set the size.
  void                size ( unsigned int rows, unsigned int columns );

  // Generate a scene.
  osg::Node *         operator()() const;

private:

  Vectors _points;
  Vectors _normals;
  unsigned int _rows;
  unsigned int _columns;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SURFACE_MESH_BUILDER_H_
