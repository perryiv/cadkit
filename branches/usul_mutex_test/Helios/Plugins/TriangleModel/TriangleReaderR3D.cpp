
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  R3D reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleReaderR3D.h"

#include "Usul/File/Stats.h"
#include "Usul/File/Contents.h"
#include "Usul/Policies/Update.h"
#include "Usul/IO/BinaryReader.h"
#include "Usul/Types/Types.h"
#include "Usul/Errors/Assert.h"

#include <fstream>
#include <stdexcept>
#include <algorithm>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderR3D::TriangleReaderR3D ( const std::string &file, Unknown *caller, TriangleDocument *doc ) :
  _file     ( file ),
  _caller   ( caller ),
  _document ( doc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderR3D::~TriangleReaderR3D()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file. 
//
//  Note: even though the VTK file holds tri-strip information, there are so 
//  many of them that, at least with OSG, it will probably be faster to draw 
//  individual triangles, which will use glDrawArrays() internally.
//
//  Don't think OSG supports packing the tri-strips end-to-end in a single 
//  osg::Geometry and passing this to OpenGL in an efficient way.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderR3D::operator()()
{
  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  typedef std::vector < osg::Vec3f > Vectors;

  // Get the size of the file.
  const Usul::Types::Uint64 fileSize ( Usul::File::size ( _file ) );

  // Open the binary file.
  std::ifstream in ( _file.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2260991012: Failed to open binary file: " + _file );

  // Feedback.
  _document->setStatusBar ( "Reading R3D triangle data..." );

  // Check the header.
  this->_check ( in, "#" );
  this->_check ( in, "vtk" );
  this->_check ( in, "DataFile" );
  this->_check ( in, "Version" );
  this->_skipLine ( in );
  this->_skipLine ( in );
  this->_check ( in, "binary" );
  this->_check ( in, "dataset" );
  this->_check ( in, "polydata" );
  this->_check ( in, "points" );

  // Read the number of points.
  unsigned int numPoints ( 0 );
  in >> numPoints;

  // Should be floating point data.
  this->_check ( in, "float" );
  this->_skipLine ( in );

  // Make a vertex pool.
  Vectors pool ( numPoints );

  // Needed in the loop.
  Usul::Types::Float32 x, y, z;

  // Update progress bar every second.
  Usul::Policies::TimeBased elapsed ( 1000 );

  // Start reading the vertices.
  for ( unsigned int i = 0; i < numPoints; ++i )
  {
    Usul::IO::Binary::ReadBigEndian::read ( in, x, y, z );
    pool.at(i).set ( x, y, z );
  }

  // Make sure it is a collection of tri-strips.
  this->_check ( in, "triangle_strips" );
  Usul::Types::Uint32 numStrips, dummy;
  in >> numStrips >> dummy;
  this->_skipLine ( in );

  // Needed in the loop.
  typedef std::vector < unsigned int > Indices;
  typedef std::vector < Indices > Strips;
  Strips strips ( numStrips );
  unsigned int numTriangles ( 0 );

  // Start reading the strips.
  for ( unsigned int j = 0; j < numStrips; ++j )
  {
    // Read how many indices are in this strip.
    Usul::Types::Uint32 numIndices ( 0 );
    Usul::IO::Binary::ReadBigEndian::read ( in, numIndices );
    strips.at(j).resize ( numIndices );
    numTriangles += ( numIndices - 2 );

    // Loop through all the indices.
    for ( unsigned int k = 0; k < numIndices; ++k )
    {
      // Read the index.
      Usul::Types::Uint32 index ( 0 );
      Usul::IO::Binary::ReadBigEndian::read ( in, index );
      strips.at(j).at(k) = index;
    }

    // Show progress.
    _document->setProgressBar ( elapsed(), in, fileSize, _caller );
  }

  // Reserve enough room
  _document->reserveTriangles ( _document->numTriangles() + numTriangles );

  // Feedback.
  _document->setStatusBar ( "Finding triangles..." );

  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  // Set all the vertices and normals.
  for ( unsigned int s = 0; s < numStrips; ++s )
  {
    // Loop through each strip.
    const Indices &indices = strips.at(s);
    const unsigned int numIndices ( indices.size() );
    const unsigned int stop ( numIndices - 2 );
    for ( unsigned int k = 0; k < stop; ++k )
    {
      // Get the three indices.
      const unsigned int &i0 = indices.at(k);
      const unsigned int &i1 = indices.at(k+1);
      const unsigned int &i2 = indices.at(k+2);

      // Get the three vertices.
      const osg::Vec3 &v0 = pool.at(i0);
      const osg::Vec3 &v1 = pool.at(i1);
      const osg::Vec3 &v2 = pool.at(i2);

      // Calculate the normal. Every other one we flip.
      const osg::Vec3 v01 ( v1 - v0 );
      const osg::Vec3 v02 ( v2 - v0 );
      osg::Vec3 n ( ( k % 2 ) ? v02 ^ v01 : v01 ^ v02 );
      n.normalize();

      // Add the triangle
      _document->addTriangle ( v0, v1, v2, n, false, true );
    }

    // Show progress.
    _document->setProgressBar ( elapsed(), s, numStrips, _caller );
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the next word read is the same as the given word.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderR3D::_check ( std::istream &in, const std::string &s ) const
{
  std::string a, b ( s );
  in >> a;
  std::transform ( a.begin(), a.end(), a.begin(), ::tolower );
  std::transform ( b.begin(), b.end(), b.begin(), ::tolower );
  if ( a != b )
  {
    std::ostringstream out;
    out << "Error 2040984431: Invalid word '" << a << "' found at beginning of R3D file.";
      throw std::runtime_error ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip the next word.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderR3D::_skipWord ( std::istream &in ) const
{
  std::string a;
  in >> a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip the next line.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderR3D::_skipLine ( std::istream &in ) const
{
  const unsigned int size ( 1023 );
  char buf[size+1];
  in.getline ( buf, size );
}
