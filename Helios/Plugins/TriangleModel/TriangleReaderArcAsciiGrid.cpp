
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizone State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Arc ASCII Grid reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleReaderArcAsciiGrid.h"

#include "Usul/File/Stats.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Predicates/CloseFloat.h"
#include "Usul/Policies/Update.h"

#include "osg/Vec3"

#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
//
//  Constants with file-scope.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  const unsigned int _milliseconds ( 250 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderArcAsciiGrid::TriangleReaderArcAsciiGrid ( const std::string &file, Usul::Interfaces::IUnknown *caller, TriangleDocument *doc ) :
  _file      ( file, Usul::File::size ( file ) ),
  _caller    ( caller ),
  _document  ( doc ),
  _progress  ( 0, 1 )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderArcAsciiGrid::~TriangleReaderArcAsciiGrid()
{
  // Need to do this because of circular references. Otherwise there are leaks.
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderArcAsciiGrid::operator()()
{
  // Clear accumulated state.
  this->clear();

  // Read the file.
  this->_read();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Note: we do not clear the reference-counted objects because 
//  we hand those to the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderArcAsciiGrid::clear()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper class for reading a stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  struct ReadStream
  {
    ReadStream ( const std::string &file, std::istream &in ) : _file ( file ), _in ( in )
    {
    }
    ReadStream ( const ReadStream &r ) : _file ( r._file ), _in ( r._in )
    {
    }
    template < class T > void operator () ( T &t )
    {
      if ( _in.eof() )
      {
        throw std::runtime_error ( "Error 3191797615: Unexpected end of file when reading: " + _file );
      }
      _in >> t;
    }
  private:
    ReadStream &operator = ( const ReadStream & );
    std::string _file;
    std::istream &_in;
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderArcAsciiGrid::_read()
{
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  in.open ( _file.first.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1099205557: Failed to open file: " + _file.first );

  // Initialize local vaiables.
  Usul::Math::Vec2ui gridSize ( 0, 0 );
  Usul::Math::Vec2d lowerLeft ( 0, 0 );
  double cellSize ( 0 );
  double noDataValue ( 0 );

  // Read header file.
  {
    std::string label;
    in >> label >> gridSize[1];  // Number of columns
    in >> label >> gridSize[0];  // Number of rows
    in >> label >> lowerLeft[0]; // Lower left x coordinate
    in >> label >> lowerLeft[1]; // Lower left y coordinate
    in >> label >> cellSize;
    in >> label >> noDataValue;
  }

  // Useful typedefs.
  typedef std::vector<float> Values;
  typedef osg::Vec3f Vec3;
  typedef Usul::Predicates::CloseFloat<double> Close;

  // More local variables.
  const unsigned int numPoints ( gridSize[0] * gridSize[1] );
  Vec3 a0 ( 0, 0, 0 ), a1 ( 0, 0, 0 ), b0 ( 0, 0, 0 ), b1 ( 0, 0, 0 );
  Close close ( 5 ); // Reasonable?
  Usul::Policies::TimeBased update ( 1000 ); // Every second.

  // Allocate two rows.
  Values row0 ( gridSize[1] );
  Values row1 ( gridSize[1] );

  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numPoints;

  // Reserve triangles.
  _document->reserveTriangles ( gridSize[0] * gridSize[1] );

  // Grab the first row.
  std::for_each ( row0.begin(), row0.end(), Helper::ReadStream ( _file.first, in ) );

  // Loop until we've read all the points.
  const unsigned int stopR ( gridSize[0] - 1 );
  for ( unsigned int i = 0; i < stopR; ++i )
  {
    // Grab the next row.
    std::for_each ( row1.begin(), row1.end(), Helper::ReadStream ( _file.first, in ) );

    // Shortcuts.
    const Values::value_type y0 ( lowerLeft[1] + ( ( i     ) * cellSize ) );
    const Values::value_type y1 ( lowerLeft[1] + ( ( i + 1 ) * cellSize ) );

    // Loop over the columns.
    const unsigned int stopC ( gridSize[1] - 1 );
    for ( unsigned int j = 0; j < stopC; ++j )
    {
      // Shortcuts.
      const Values::value_type xa ( lowerLeft[0] + ( ( j     ) * cellSize ) );
      const Values::value_type xb ( lowerLeft[0] + ( ( j + 1 ) * cellSize ) );

      // Calculate vertices of current triangles.
      const Vec3 a ( xa, y1, row1.at ( j     ) );
      const Vec3 b ( xb, y1, row1.at ( j + 1 ) );
      const Vec3 c ( xa, y0, row0.at ( j     ) );
      const Vec3 d ( xb, y0, row0.at ( j + 1 ) );

      // See if points are close to "no data".
      if ( false == close ( b[2], noDataValue ) &&
           false == close ( c[2], noDataValue ) )
      {
        // Add first triangle.
        if ( false == close ( a[2], noDataValue ) )
        {
          Vec3 n ( ( b - a ) ^ ( c - a ) );
          n.normalize();
          OsgTools::Triangles::Triangle::RefPtr t ( _document->addTriangle ( a, b, c, n, false ) );
          t->original ( true );
        }

        // Add second triangle.
        if ( false == close ( d[2], noDataValue ) )
        {
          Vec3 n ( ( d - b ) ^ ( c - b ) );
          n.normalize();
          OsgTools::Triangles::Triangle::RefPtr t ( _document->addTriangle ( b, d, c, n, false ) );
          t->original ( true );
        }
      }

      // Feedback.
      this->_incrementProgress ( update() );
    }

    // Swap rows.
    row0.swap ( row1 );
  }

  // Trim the size down.
  _document->purge();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderArcAsciiGrid::_incrementProgress ( bool state )
{
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  _document->setProgressBar ( state, numerator, denominator );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}
