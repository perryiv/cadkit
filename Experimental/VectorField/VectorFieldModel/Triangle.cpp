
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Triangle.h"

#include "Usul/Containers/Array2D.h"
#include "Usul/Errors/ThrowingPolicy.h"

#include "GN/Math/Matrix.h"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Triangle::Triangle ( const Indices& indices, Positions* positions, Vectors* vectors ) : BaseClass (),
_positions ( positions ),
_vectors   ( vectors ),
_indices   ( indices ),
_a (),
_b ()
{
  this->_findMatrixValues();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Triangle::~Triangle()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Calculate the determinant of a 3x3.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Matrix >
  double determinant ( const Matrix& m )
  {
    return ( ( ( m ( 0, 0 ) * m ( 1, 1 ) * m ( 2, 2 ) ) +
               ( m ( 0, 1 ) * m ( 1, 2 ) * m ( 2, 0 ) ) +
               ( m ( 0, 2 ) * m ( 1, 0 ) * m ( 2, 1 ) ) ) -
             ( ( m ( 2, 0 ) * m ( 1, 1 ) * m ( 0, 2 ) ) +
               ( m ( 2, 1 ) * m ( 1, 2 ) * m ( 0, 0 ) ) +
               ( m ( 2, 2 ) * m ( 1, 0 ) * m ( 0, 1 ) ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make sure the float is finite.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > struct FloatTester
  {
    static bool finite ( const T &v )
    {
      return ::Usul::Math::finite ( v );
    }
  };
}

///////////////////////////////////////////////////////////////////////////////
//
//  Find A and B values.
//
///////////////////////////////////////////////////////////////////////////////

void Triangle::_findMatrixValues()
{
  Position p0 ( _positions->at ( _indices[0] ) );
  Position p1 ( _positions->at ( _indices[1] ) );
  Position p2 ( _positions->at ( _indices[2] ) );

  Vector v0 ( _vectors->at ( _indices[0] ) );
  Vector v1 ( _vectors->at ( _indices[1] ) );
  Vector v2 ( _vectors->at ( _indices[2] ) );

  typedef unsigned int                            SizeType;
  typedef std::vector < SizeType >                SizeContainer;
  typedef std::vector < std::vector < double > >  MatrixContainer;
  typedef Detail::FloatTester < double >          DependentTester;
  typedef ::Usul::Errors::ThrowingPolicy < std::runtime_error > ErrorCheckerType;
  typedef GN::Math::Matrix < SizeType, MatrixContainer, SizeContainer, DependentTester, ErrorCheckerType > Matrix;

  {
    SizeContainer pivots;
    Matrix matrix ( 3, 3 );

    matrix ( 0, 0 ) = p0[0];
    matrix ( 0, 1 ) = p0[1];
    matrix ( 0, 2 ) = 1;

    matrix ( 1, 0 ) = p1[0];
    matrix ( 1, 1 ) = p1[1];
    matrix ( 1, 2 ) = 1;

    matrix ( 2, 0 ) = p2[0];
    matrix ( 2, 1 ) = p2[1];
    matrix ( 2, 2 ) = 1;

    // Perform the LU decomposition.
    matrix.luDecomp ( pivots );

    std::vector < double > b ( 3 );
    b[0] = v0[0];
    b[1] = v1[0];
    b[2] = v2[0];

    // Solve (Ax=b).
    matrix.luSolve ( pivots, b );

    _a[0] = b[0];
    _a[1] = b[1];
    _b[0] = b[2];

    b[0] = v0[1];
    b[1] = v1[1];
    b[2] = v2[1];

    // Solve (Ax=b).
    matrix.luSolve ( pivots, b );

    _a[2] = b[0];
    _a[3] = b[1];
    _b[1] = b[2];
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the indices.
//
///////////////////////////////////////////////////////////////////////////////

const Triangle::Indices& Triangle::indices() const
{
  return _indices;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the barycentric coordinates.
//  http://www.farinhansford.com/dianne/teaching/cse470/materials/BarycentricCoords.pdf
//
///////////////////////////////////////////////////////////////////////////////

Usul::Math::Vec3d Triangle::barycentric( const Position& p ) const
{
  Usul::Math::Vec3d bc;

  Position p0 ( _positions->at ( _indices[0] ) );
  Position p1 ( _positions->at ( _indices[1] ) );
  Position p2 ( _positions->at ( _indices[2] ) );

  Usul::Containers::Array2D < double > a ( 3, 3 );

  a ( 0, 0 ) = p0[0];
  a ( 1, 0 ) = p0[1];
  a ( 2, 0 ) = 1;

  a ( 0, 1 ) = p1[0];
  a ( 1, 1 ) = p1[1];
  a ( 2, 1 ) = 1;

  a ( 0, 2 ) = p2[0];
  a ( 1, 2 ) = p2[1];
  a ( 2, 2 ) = 1;

  const double A ( Detail::determinant ( a ) );

  a ( 0, 0 ) = p[0];
  a ( 1, 0 ) = p[1];
  a ( 2, 0 ) = 1;

  a ( 0, 1 ) = p1[0];
  a ( 1, 1 ) = p1[1];
  a ( 2, 1 ) = 1;

  a ( 0, 2 ) = p2[0];
  a ( 1, 2 ) = p2[1];
  a ( 2, 2 ) = 1;

  const double A0 ( Detail::determinant ( a ) );

  a ( 0, 0 ) = p0[0];
  a ( 1, 0 ) = p0[1];
  a ( 2, 0 ) = 1;

  a ( 0, 1 ) = p[0];
  a ( 1, 1 ) = p[1];
  a ( 2, 1 ) = 1;

  a ( 0, 2 ) = p2[0];
  a ( 1, 2 ) = p2[1];
  a ( 2, 2 ) = 1;

  const double A1 ( Detail::determinant ( a ) );

  a ( 0, 0 ) = p0[0];
  a ( 1, 0 ) = p0[1];
  a ( 2, 0 ) = 1;

  a ( 0, 1 ) = p1[0];
  a ( 1, 1 ) = p1[1];
  a ( 2, 1 ) = 1;

  a ( 0, 2 ) = p[0];
  a ( 1, 2 ) = p[1];
  a ( 2, 2 ) = 1;

  const double A2 ( Detail::determinant ( a ) );

  bc.set ( A0 / A, A1 / A, A2 / A );

  return bc;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Is the point inside?
//
///////////////////////////////////////////////////////////////////////////////

bool Triangle::pointInside ( const Position& p ) const
{
  Usul::Math::Vec3d bc ( this->barycentric ( p ) );

  bool b0 ( bc[0] <= 1 && bc[0] >= 0 );
  bool b1 ( bc[1] <= 1 && bc[1] >= 0 );
  bool b2 ( bc[2] <= 1 && bc[2] >= 0 );

  return b0 && b1 && b2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the vector at this point.
//
///////////////////////////////////////////////////////////////////////////////

Triangle::Vector Triangle::vectorAtPoint ( const Position& p ) const
{
  Vector v0 ( _vectors->at ( _indices[0] ) );
  Vector v1 ( _vectors->at ( _indices[1] ) );
  Vector v2 ( _vectors->at ( _indices[2] ) );

  Usul::Math::Vec3d bc ( this->barycentric ( p ) );

  Vector v0p ( v0 * static_cast < Vector::value_type > ( bc[0] ) );
  Vector v1p ( v1 * static_cast < Vector::value_type > ( bc[1] ) );
  Vector v2p ( v2 * static_cast < Vector::value_type > ( bc[2] ) );

  return v0p + v1p + v2p;
}
