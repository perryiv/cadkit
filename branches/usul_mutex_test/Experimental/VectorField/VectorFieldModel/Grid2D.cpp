
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Grid2D.h"

#include "Usul/Adaptors/Random.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Grid2D::Grid2D( ) :
_x ( 0 ),
_y ( 0 ),
_points ( new Positions ),
_vectors ( new Vectors ),
_cells ( 0, 0 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Grid2D::Grid2D( unsigned int r, unsigned int c ) :
_x ( r ),
_y ( c ),
_points ( new Positions ),
_vectors ( new Vectors ),
_cells ( r, c )
{
  const unsigned int numPoints ( ( r + 1 ) * ( c + 1 ) );
  _points->reserve ( numPoints );
  _vectors->reserve ( numPoints );

  Usul::Adaptors::Random<Position::value_type> random ( -0.25, 0.25 );

  // Make some vectors.
  for ( unsigned int i = 0; i < r + 1; ++i )
  {
    for ( unsigned int j = 0; j < c + 1; ++j )
    {
      float x ( static_cast < float > ( static_cast < int > ( j ) - 10.0 ) + random() );
      float y ( static_cast < float > ( static_cast < int > ( r + 1 - i ) - 10.0 ) + random() );

      x /= 5.0;
      y /= 5.0;

      Position p ( x, y, 0.0 );
      Vector d ( -y, x );

#if 0
      float x0 ( ( x + 0.8 ) * Usul::Math::cos ( 0.4 * y ) );
      float y0 ( ( y + 0.8 ) * Usul::Math::cos ( 0.4 * x ) );
      d[0] = x0;
      d[1] = y0;
#endif
      d[0] = -0.103209 + ( 0.051511 * x ) - ( 0.302688 * y ) + ( 0.037556 * x * y ) - ( 0.232875 * x * x ) + ( 0.611528 * y * y );
      d[1] =  0.143656 + ( 0.687847 * x ) - ( 0.144779 * y ) - ( 0.213010 * x * y ) - ( 1.029676 * x * x ) + ( 0.246278 * y * y );

      _points->push_back ( p );
      _vectors->push_back ( d );
    }
  }

  // Make some cells.
  for ( unsigned int i = 0; i < r; ++i )
  {
    for ( unsigned int j = 0; j < c; ++j )
    {
      Cell::Indices indices;

      // Set the indices.
      indices[Cell::UPPER_LEFT]  = i         * ( r + 1 ) + j;
      indices[Cell::UPPER_RIGHT] = i         * ( r + 1 ) + j + 1;
      indices[Cell::LOWER_LEFT]  = ( i + 1 ) * ( r + 1 ) + j;
      indices[Cell::LOWER_RIGHT] = ( i + 1 ) * ( r + 1 ) + j + 1;

      Cell::RefPtr c ( new Cell ( indices, _points.get(), _vectors.get() ) );
      _cells.at ( i, j ) = c;
    }
  }

  this->removeCell ( 4, 5 );
  this->removeCell ( 5, 5 );
  this->removeCell ( 5, 6 );
  this->removeCell ( 5, 7 );
  this->removeCell ( 5, 8 );
  this->removeCell ( 5, 9 );
  this->removeCell ( 6, 5 );
  this->removeCell ( 6, 6 );
  this->removeCell ( 6, 7 );
  this->removeCell ( 7, 5 );
  this->removeCell ( 8, 5 );
  this->removeCell ( 9, 5 );

  this->removeCell ( 9, 11 );
  this->removeCell ( 9, 12 );

  /*this->removeCell ( 10, 10 );
  this->removeCell ( 10, 11 );
  this->removeCell ( 10, 12 );
  this->removeCell ( 10, 13 );

  this->removeCell ( 11, 11 );
  this->removeCell ( 11, 12 );*/

  /*this->removeCell ( 5, 16 );
  this->removeCell ( 5, 17 );
  this->removeCell ( 6, 16 );
  this->removeCell ( 6, 17 );
  this->removeCell ( 4, 18 );
  this->removeCell ( 5, 18 );*/
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the i'th point.
//
///////////////////////////////////////////////////////////////////////////////

void Grid2D::point( unsigned int i, const Position& v )
{
  _points->at ( i ) = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th point.
//
///////////////////////////////////////////////////////////////////////////////

const Grid2D::Position& Grid2D::point( unsigned int i ) const
{
  return _points->at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the i'th vector.
//
///////////////////////////////////////////////////////////////////////////////

void Grid2D::vector( unsigned int i, const Vector& v )
{
  _vectors->at ( i ) = v;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the i'th vector.
//
///////////////////////////////////////////////////////////////////////////////

const Grid2D::Vector& Grid2D::vector( unsigned int i ) const
{
  return _vectors->at ( i );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell at index i, j
//
///////////////////////////////////////////////////////////////////////////////

const Grid2D::Cell* Grid2D::cell( unsigned int i, unsigned int j ) const
{
  return _cells.at ( i, j ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the cell at index i, j
//
///////////////////////////////////////////////////////////////////////////////

Grid2D::Cell* Grid2D::cell( unsigned int i, unsigned int j )
{
  return _cells.at ( i, j ).get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get cell at location x, y.
//
///////////////////////////////////////////////////////////////////////////////

const Grid2D::Cell* Grid2D::cellAtLocation ( double x, double y ) const
{
  Position p ( x, y, 0.0 );

  for ( unsigned int i = 0; i < _x; ++i )
  {
    for ( unsigned int j = 0; j < _y; ++j )
    {
      Cell::RefPtr c ( _cells.at ( i, j ) );
      
      if ( c.valid() )
      {
        if ( c->upper()->pointInside ( p ) || c->lower()->pointInside ( p ) )
          return c.get();
      }
    }
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get triangle at location x, y.
//
///////////////////////////////////////////////////////////////////////////////

const Triangle* Grid2D::triangleAtLocation ( double x, double y ) const
{
  Position p ( x, y, 0.0 );

  for ( unsigned int i = 0; i < _x; ++i )
  {
    for ( unsigned int j = 0; j < _y; ++j )
    {
      Cell::RefPtr c ( _cells.at ( i, j ) );
      
      if ( c.valid() )
      {
        if ( c->upper()->pointInside ( p ) )
          return c->upper();

        if ( c->lower()->pointInside ( p ) )
          return c->lower();
      }
    }
  }

  return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of rows.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Grid2D::rows() const
{
  return _x;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of columns.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Grid2D::columns() const
{
  return _y;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the number of points.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int Grid2D::numPoints() const
{
  return _points->size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove cell.
//
///////////////////////////////////////////////////////////////////////////////

void Grid2D::removeCell ( unsigned int i, unsigned int j )
{
  Cell::RefPtr cell ( _cells.at ( i, j ) );
  _vectors->at ( cell->indices()[0] ) = Vector ( 0.0, 0.0 );
  _vectors->at ( cell->indices()[1] ) = Vector ( 0.0, 0.0 );
  _vectors->at ( cell->indices()[2] ) = Vector ( 0.0, 0.0 );
  _vectors->at ( cell->indices()[3] ) = Vector ( 0.0, 0.0 );
  _cells.at ( i, j ) = 0x0;
}
