
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_ALGORITHMS_MARCHING_CUBES_H__
#define __USUL_ALGORITHMS_MARCHING_CUBES_H__

#include "Usul/Bits/Bits.h"

#include "Usul/Algorithms/Detail/ConfigurationTable.h"
#include "Usul/Algorithms/Detail/Configurations.h"
#include "Usul/Algorithms/Detail/Corners.h"
#include "Usul/Algorithms/Detail/Rotations.h"

#include <stdexcept>
#include <vector>

namespace Usul {
namespace Algorithms {

  namespace Detail
  {

    template < class ValueType, class LevelType >
    void check ( ValueType v, LevelType level, unsigned int vertex, unsigned char &flags )
    {
      if ( static_cast < LevelType > ( v ) > level )
      {
        flags = Usul::Bits::add < unsigned char > ( flags, vertex );
      }
    }

    /// Helper class for interpolation of vertices.
    template < class Vertex_ >
    class InterpolateHelper
    {
    public:
      typedef Vertex_ Vertex;

      InterpolateHelper () : _vertices ()
      {
        _vertices[ Corners::V0 ] = Vertex ( 0, 0, 0 );
        _vertices[ Corners::V1 ] = Vertex ( 1, 0, 0 );
        _vertices[ Corners::V2 ] = Vertex ( 0, 1, 0 );
        _vertices[ Corners::V3 ] = Vertex ( 1, 1, 0 );
        _vertices[ Corners::V4 ] = Vertex ( 0, 0, 1 );
        _vertices[ Corners::V5 ] = Vertex ( 1, 0, 1 );
        _vertices[ Corners::V6 ] = Vertex ( 0, 1, 1 );
        _vertices[ Corners::V7 ] = Vertex ( 1, 1, 1 );
      }

    protected:
      typedef std::map < Corners::Vertices, Vertex > Vertices;
      Vertices _vertices;
    };

    /// Just picks the mid-point.
    template < class Vertex_ >
    class DiscreteInterpolator : InterpolateHelper < Vertex_ >
    {
    public:
      template < class GridValueType, class LevelType >
      Vertex operator () ( Corners::Vertices v0, Corners::Vertices v1, GridValueType t0, GridValueType t1, LevelType level )
      {
        return ( _vertices [ v0 ] + _vertices [ v1 ] ) * 0.5;
      }
    };

    /// Does a linear interpolation.
    template < class Vertex_ >
    class LinearInterpolator : InterpolateHelper < Vertex_ >
    {
    public:
      template < class GridValueType, class LevelType >
      Vertex operator () ( Corners::Vertices v0, Corners::Vertices v1, GridValueType t0, GridValueType t1, LevelType level )
      {
        typedef Vertex::value_type ValueType;

        ValueType alpha ( static_cast < ValueType > ( ( level - t0 ) / ( t1 - t0 ) ) );
        ValueType beta ( static_cast < ValueType > ( 1.0 - alpha ) );

        return ( ( _vertices [ v0 ] * static_cast < ValueType > ( beta ) ) + ( _vertices [ v1 ] * static_cast < ValueType > ( alpha ) ) );
      }
    };

  }

template < class Grid, class LevelType, class Interpolator, class TriangleAdder  >
inline void marchingCubes ( const Grid& grid, LevelType level, TriangleAdder& adder )
{
  // Typedefs.
  typedef typename Grid::SizeType SizeType;
  typedef typename Grid::ValueType ValueType;
  typedef typename Grid::CellSize CellSize;
  typedef typename TriangleAdder::Vertex Vertex;
  typedef Vertex::value_type             RealType;
  typedef Detail::Configurations::Configuration Configuration;

  // Get the dimensions.
  SizeType x ( grid.x () );
  SizeType y ( grid.y () );
  SizeType z ( grid.z () );

  // Check dimensions.
  if ( 0 == x || 0 == y || 0 == z )
    throw std::runtime_error ( "Error 7298248760: Invalid dimensions.  Must be larger than 0. " );

  // Get the cell size.
  CellSize cellSize ( grid.cellSize () );

  // Configuration table.
  Detail::ConfigurationTable table;

  // Rotation table
  Detail::Rotations rotations;

  // Triangle configuration.
  Detail::Configurations triangles;

  // Make a interpolator class.
  Interpolator interpolater;

  for ( SizeType k = 0; k < z - 1 ; ++k )
  {
    for ( SizeType j = 0; j < y - 1; ++j )
    {
      for ( SizeType i = 0; i < x - 1; ++i )
      {
        // Get the values at each vertex of the voxel.
        ValueType t0 ( grid ( i,     j,     k     ) );
        ValueType t1 ( grid ( i + 1, j,     k     ) );
        ValueType t2 ( grid ( i,     j + 1, k     ) );
        ValueType t3 ( grid ( i + 1, j + 1, k     ) );
        ValueType t4 ( grid ( i,     j,     k + 1 ) );
        ValueType t5 ( grid ( i + 1, j,     k + 1 ) );
        ValueType t6 ( grid ( i,     j + 1, k + 1 ) );
        ValueType t7 ( grid ( i + 1, j + 1, k + 1 ) );

        // Flags for which vertices are internal.
        unsigned char flags ( 0 );

        // Check each vertex.
        Detail::check ( t0, level, Detail::Corners::V0, flags );
        Detail::check ( t1, level, Detail::Corners::V1, flags );
        Detail::check ( t2, level, Detail::Corners::V2, flags );
        Detail::check ( t3, level, Detail::Corners::V3, flags );
        Detail::check ( t4, level, Detail::Corners::V4, flags );
        Detail::check ( t5, level, Detail::Corners::V5, flags );
        Detail::check ( t6, level, Detail::Corners::V6, flags );
        Detail::check ( t7, level, Detail::Corners::V7, flags );

        std::vector < ValueType > values ( 129, 0 );
        values.at ( Detail::Corners::V0 ) = t0;
        values.at ( Detail::Corners::V1 ) = t1;
        values.at ( Detail::Corners::V2 ) = t2;
        values.at ( Detail::Corners::V3 ) = t3;
        values.at ( Detail::Corners::V4 ) = t4;
        values.at ( Detail::Corners::V5 ) = t5;
        values.at ( Detail::Corners::V6 ) = t6;
        values.at ( Detail::Corners::V7 ) = t7;

        // Get the rotation and triangle configuration for these flags.
        Detail::ConfigurationTable::Entry entry ( table ( flags ) );

        unsigned char rotation ( entry.first );
        unsigned char configuration ( entry.second );

        Detail::Rotations::RotationMap map ( rotations ( rotation ) );
        Configuration triangleList ( triangles ( configuration ) );

        // Apply the rotation.
        triangles.apply ( triangleList, map );

        Vertex offset ( static_cast < RealType > ( i + cellSize [ 0 ] ), 
                        static_cast < RealType > ( j + cellSize [ 1 ] ), 
                        static_cast < RealType > ( k + cellSize [ 2 ] ) );

        /*Vertex offset ( static_cast < RealType > ( i ), 
                        static_cast < RealType > ( j ), 
                        static_cast < RealType > ( k ) );*/

        //if ( configuration == 7 )
        {
          // Make the triangles.
          for ( Configuration::iterator iter = triangleList.begin (); iter != triangleList.end (); ++iter )
          {
            Vertex i0 ( interpolater ( iter->_p0, iter->_p1, values.at ( iter->_p0 ), values.at ( iter->_p1 ), level ) );
            Vertex i1 ( interpolater ( iter->_p2, iter->_p3, values.at ( iter->_p2 ), values.at ( iter->_p3 ), level ) );
            Vertex i2 ( interpolater ( iter->_p4, iter->_p5, values.at ( iter->_p4 ), values.at ( iter->_p5 ), level ) );


            /*Vertex v0 ( offset +  Vertex ( static_cast < RealType > ( i0 [ 0 ] * cellSize [ 0 ] ), 
                                           static_cast < RealType > ( i0 [ 1 ] * cellSize [ 1 ] ), 
                                           static_cast < RealType > ( i0 [ 2 ] * cellSize [ 2 ] ) ) );

            Vertex v1 ( offset +  Vertex ( static_cast < RealType > ( i1 [ 0 ] * cellSize [ 0 ] ), 
                                           static_cast < RealType > ( i1 [ 1 ] * cellSize [ 1 ] ), 
                                           static_cast < RealType > ( i1 [ 2 ] * cellSize [ 2 ] ) ) );

            Vertex v2 ( offset +  Vertex ( static_cast < RealType > ( i2 [ 0 ] * cellSize [ 0 ] ), 
                                           static_cast < RealType > ( i2 [ 1 ] * cellSize [ 1 ] ), 
                                           static_cast < RealType > ( i2 [ 2 ] * cellSize [ 2 ] ) ) );*/

            Vertex v0 ( offset + i0 );
            Vertex v1 ( offset + i1 );
            Vertex v2 ( offset + i2 );

            adder ( v0, v1, v2 );
          }
        }
      }
    }
  }
}

}
}


#endif // __USUL_ALGORITHMS_MARCHING_CUBES_H__
