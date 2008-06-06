
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __VECTOR_FIELD_GRID_TWO_D_H__
#define __VECTOR_FIELD_GRID_TWO_D_H__

#include "Cell2D.h"

#include "Usul/Containers/Array2D.h"

#include "osg/Array"

#include <vector>

class Triangle;

class Grid2D
{
public:
  typedef osg::Vec3Array              Positions;
  typedef Positions::value_type       Position;
  typedef osg::ref_ptr < Positions >  PositionsPtr;
  typedef osg::Vec2Array              Vectors;
  typedef Vectors::value_type         Vector;
  typedef osg::ref_ptr < Vectors >    VectorsPtr;

  typedef Cell2D                                       Cell;
  typedef Usul::Containers::Array2D < Cell::RefPtr >   Cells;

  Grid2D ();
  Grid2D( unsigned int x, unsigned int y );

  /// Remove cell.
  void              removeCell ( unsigned int i, unsigned int j );

  /// Get the number of points.
  unsigned int      numPoints () const;

  /// Get/Set the i'th point.
  void              point( unsigned int i, const Position& v );
  const Position&   point( unsigned int i ) const;

  /// Get/Set the i'th vector.
  void              vector( unsigned int i, const Vector& v );
  const Vector&     vector( unsigned int i ) const;

  /// Get the cell at index i, j
  const Cell*       cell( unsigned int i, unsigned int j ) const;
  Cell*             cell( unsigned int i, unsigned int j );

  /// Get cell at location x, y.
  const Cell*       cellAtLocation ( double x, double y ) const;

  /// Get triangle at location x, y.
  const Triangle*   triangleAtLocation ( double x, double y ) const;

  /// Get the size.
  unsigned int      rows() const;
  unsigned int      columns() const;

private:
  unsigned int _x;
  unsigned int _y;
  PositionsPtr _points;
  VectorsPtr   _vectors;
  Cells _cells;
};


#endif 
