
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

#include "OsgTools/Triangles/Exceptions.h"

#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Path.h"
#include "Usul/IO/BinaryReader.h"
#include "Usul/IO/BinaryWriter.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Predicates/FileExists.h"

#include "osg/Vec3"

#include <algorithm>
#include <exception>
#include <fstream>
#include <list>
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
  _progress  ( 0, 1 ),
  _textureFilename( "" )
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
  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  // Clear accumulated state.
  this->clear();

  // Read the file.
  this->_read();

  // Load the texture file
  this->_loadTexture ( _textureFilename );
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
//  Helper function to write a triangle to the stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Stream, class Vertex, class Normal > 
  inline void writeTriangle ( Stream &stream, const Vertex &v0, const Vertex &v1, const Vertex &v2, const Normal &n )
  {
    Usul::IO::Binary::WriteSystemEndian::write3 ( stream, v0 );
    Usul::IO::Binary::WriteSystemEndian::write3 ( stream, v1 );
    Usul::IO::Binary::WriteSystemEndian::write3 ( stream, v2 );
    Usul::IO::Binary::WriteSystemEndian::write3 ( stream, n  );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Helper function to read a triangle from the stream.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  template < class Stream, class Vertex, class Normal > 
  inline void readTriangle ( Stream &stream, Vertex &v0, Vertex &v1, Vertex &v2, Normal &n )
  {
    Usul::IO::Binary::ReadSystemEndian::read3 ( stream, v0 );
    Usul::IO::Binary::ReadSystemEndian::read3 ( stream, v1 );
    Usul::IO::Binary::ReadSystemEndian::read3 ( stream, v2 );
    Usul::IO::Binary::ReadSystemEndian::read3 ( stream, n  );
  }
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

  _textureFilename = Usul::File::directory( _file.first, true ) + Usul::File::base( _file.first ) + ".png";

  // Initialize local vaiables.
  Usul::Math::Vec2ui gridSize ( 0, 0 );
  Usul::Math::Vec2d lowerLeft ( 0, 0 );
  double cellSize ( 0 );
  double noDataDouble ( 0 );

  // Read header file.
  {
    std::string label;
    in >> label >> gridSize[1];  // Number of columns
    in >> label >> gridSize[0];  // Number of rows
    in >> label >> lowerLeft[0]; // Lower left x coordinate
    in >> label >> lowerLeft[1]; // Lower left y coordinate
    in >> label >> cellSize;
    in >> label >> noDataDouble;

    _header.cols = gridSize[1];
    _header.rows = gridSize[0];
    _header.west = lowerLeft[0];
    _header.south = lowerLeft[1];
    _header.ew_resol = cellSize;
    _header.ns_resol = cellSize;
    
  }

  // Make sure nodata value is an integer.
  const int noDataInt ( static_cast < int > ( noDataDouble ) );
  USUL_ASSERT ( noDataDouble == static_cast < double > ( noDataInt ) );

  // Useful typedefs.
  typedef std::vector<float> Values;
  typedef osg::Vec3f Vec3;
  typedef Usul::Predicates::CloseFloat<double> Close;

  // More local variables.
  const unsigned int numPoints ( gridSize[0] * gridSize[1] );
  Vec3 a0 ( 0, 0, 0 ), a1 ( 0, 0, 0 ), b0 ( 0, 0, 0 ), b1 ( 0, 0, 0 );
  Usul::Policies::TimeBased update ( 1000 ); // Every second.

  // Allocate two rows.
  Values row0 ( gridSize[1] );
  Values row1 ( gridSize[1] );

  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = numPoints;

  // Feedback.
  std::cout << Usul::Strings::format ( "Grid Size X: ", gridSize[0], ", Y: ", gridSize[1], ", Total: ", numPoints, '\n',
                                       "Lower Left: ", lowerLeft[0], ", ", lowerLeft[1], '\n', 
                                       "Cell Size: ", cellSize, '\n',
                                       "No Data: ", noDataInt ) << std::endl;

  // Make a temp file to hold all the valid triangles.
  Usul::File::Temp validTriangles ( Usul::File::Temp::BINARY );
  std::cout << Usul::Strings::format ( "Making temporary file '", validTriangles.name(), "' to hold valid triangles" ) << std::endl;

  // When done parsing, holds the number valid triangles.
  unsigned int count ( 0 );

  std::cout << Usul::Strings::format ( "Reading rows from: ", _file.first, " ..." ) << std::endl;

  // Grab the first row.
  std::for_each ( row0.begin(), row0.end(), Helper::ReadStream ( _file.first, in ) );

  // Loop until we've read all the points.
  const unsigned int stopR ( gridSize[0] - 1 );
  for ( unsigned int i = 0; i < stopR; ++i )
  {
    // Grab the next row.
    std::for_each ( row1.begin(), row1.end(), Helper::ReadStream ( _file.first, in ) );

    // Shortcuts.
    const Values::value_type xa ( lowerLeft[1] + ( ( i     ) * cellSize ) );
    const Values::value_type xb ( lowerLeft[1] + ( ( i + 1 ) * cellSize ) );

    // Loop over the columns.
    const unsigned int stopC ( gridSize[1] - 1 );
    for ( unsigned int j = 0; j < stopC; ++j )
    {
      // Shortcuts.
      const Values::value_type y0 ( lowerLeft[0] + ( ( j     ) * cellSize ) );
      const Values::value_type y1 ( lowerLeft[0] + ( ( j + 1 ) * cellSize ) );

      // Calculate vertices of current triangles.
      const Vec3 a ( xa, y0, row0.at ( j     ) );
      const Vec3 b ( xa, y1, row0.at ( j + 1 ) );
      const Vec3 c ( xb, y0, row1.at ( j     ) );
      const Vec3 d ( xb, y1, row1.at ( j + 1 ) );

      // See if points are "no data".
      if ( ( static_cast < int > ( d[2] ) != noDataInt ) &&
           ( static_cast < int > ( a[2] ) != noDataInt ) )
      {
        // Add first triangle.
        if ( static_cast < int > ( c[2] != noDataInt ) )
        {
          Vec3 n ( ( d - c ) ^ ( a - c ) );
          n.normalize();
          //_triangleProxies.push_back ( TriangleProxy ( a, c, d, n ) );
          Helper::writeTriangle ( validTriangles.stream(), a, c, d, n );
          ++count;
        }

        // Add second triangle.
        if ( static_cast < int > ( b[2] != noDataInt ) )
        {
          Vec3 n ( ( b - d ) ^ ( a - d ) );
          n.normalize();
          //_triangleProxies.push_back ( TriangleProxy ( a, d, b, n ) );
          Helper::writeTriangle ( validTriangles.stream(), a, d, b, n );
          ++count;
        }
      }

      if ( update() )
      {
        std::cout << Usul::Strings::format ( "Added ", count, " triangles so far..." ) << std::endl;
      }

      // Feedback.
      this->_incrementProgress ( update() );
    }

    // Swap rows.
    row0.swap ( row1 );
  }

  // Close the input file.
  in.close();

  // Close (but don't delete) the temporary file.
  validTriangles.stream().flush();
  validTriangles.close();

  // Delete the rows.
  { Values temp; std::swap ( row0, temp ); }
  { Values temp; std::swap ( row1, temp ); }

  // Reserve triangles. Reduce size until it works.
  unsigned int numTrianglesToAllocate ( count );
  while ( numTrianglesToAllocate > 0 )
  {
    try
    {
      _document->reserveTriangles ( numTrianglesToAllocate );
      std::cout << Usul::Strings::format ( "Allocated ", numTrianglesToAllocate, " triangles" ) << std::endl;
      break;
    }
    catch ( const std::bad_alloc & )
    {
      std::cout << Usul::Strings::format ( "Failed to allocate ", numTrianglesToAllocate, " triangles" ) << std::endl;
      numTrianglesToAllocate = static_cast < unsigned int > ( static_cast < double > ( numTrianglesToAllocate ) * 0.9 );
    }
  }

  {
    // Open the temporary file.
    std::ifstream in2;
    in2.rdbuf()->pubsetbuf ( buffer, bufSize );
    in2.open ( validTriangles.name().c_str(), std::ofstream::out | std::ofstream::binary );
    if ( !in2.is_open() )
      throw std::runtime_error ( "Error 1199892810: Failed to open temporary triangle file: " + validTriangles.name() );

    // Space for the triangles.
    Vec3 v0 ( 0, 0, 0 );
    Vec3 v1 ( 0, 0, 0 );
    Vec3 v2 ( 0, 0, 0 );
    Vec3 n  ( 0, 0, 0 );

    // Set the progress numbers.
    _progress.first = 0;
    _progress.second = numTrianglesToAllocate;

    // No memory pool. These files can get really big.
    _document->usePool ( false );

    // Read and add triangles.
    for ( unsigned int k = 0; k < numTrianglesToAllocate; ++k )
    {
      if ( update() )
      {
        std::cout << Usul::Strings::format ( "Reading triangle ", k, " of ", numTrianglesToAllocate ) << std::endl;
      }

      // This could throw if the binary file did not get all the 
      // valid triangles, which happens when the disk fills up.
      try
      {
        // Read triangle.
        Helper::readTriangle ( in2, v0, v1, v2, n );
      }
      catch ( const Usul::Exceptions::IO::UnexpectedEndOfFile & )
      {
        std::cout << Usul::Strings::format ( "Error 3718606332: Unexpected end of file reached while parsing: ", validTriangles.name(), ". Proceeding with the triangles already added." ) << std::endl;
        break;
      }

      // This could throw if the file is huge.
      try
      {
        // Add triangle.
        OsgTools::Triangles::Triangle::RefPtr t ( _document->addTriangle ( v0, v1, v2, n, false, true ) );
        t->original ( true );
      }
      catch ( const OsgTools::Triangles::Exceptions::TriangleVerticesEqual & )
      {
        std::cout << Usul::Strings::format ( "Error 3597395729: Two or more triangle vertices are equal: v0 = ", v0[0], ' ', v0[1], ' ', v0[2], ", v1 = ", v1[0], ' ', v1[1], ' ', v1[2], ", v2 = ", v2[0], ' ', v2[1], ' ', v2[2] ) << std::endl;
        break;
      }
      catch ( const std::bad_alloc & )
      {
        std::cout << Usul::Strings::format ( "Error 3718606332: Failed to allocate memory for triangle ", k, " of ", numTrianglesToAllocate, ". Proceeding with the triangles already added." ) << std::endl;
        break;
      }

      // Feedback.
      this->_incrementProgress ( update() );
    }
  }
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
  _document->setProgressBar ( state, numerator, denominator, _caller );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Load an image file to use as a texture file to the model.
//  An actual texture object is not created.  Instead, we use
//  the color information contained in the image to set the 
//  corresponding color at a given point in the terrain file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderArcAsciiGrid::_loadTexture ( const std::string& filename )
{

  std::cout << "Checking for texture with name " << filename << std::endl;
  if( true != Usul::Predicates::FileExists::test( filename ) )
  {
      if ( 0x0 != _document )
       _document->useMaterial ( true );
      std::cout << "Texture file not found.  Using default material color." << std::endl;
      return;
  }

  std::cout << "Texture file: " << filename << " found.  Loading texture file..." << std::endl;
  // Get interface to triangle set for loading a color file
  Usul::Interfaces::ILoadColorFile::QueryPtr colorFile ( _document );


  // Create header vector from header information
  std::vector< float > header ( 0 );

  header.push_back( _header.rows );
  header.push_back( _header.cols );
  header.push_back( _header.west );
  header.push_back( _header.south );
  header.push_back( _header.ns_resol );
  
  colorFile->loadColorFile( filename, header );
}

