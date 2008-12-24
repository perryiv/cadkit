
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  STL reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleReaderSTL.h"

#include "Usul/File/Stats.h"
#include "Usul/File/Contents.h"
#include "Usul/Policies/Update.h"
#include "Usul/IO/BinaryReader.h"
#include "Usul/Types/Types.h"
#include "Usul/Errors/Assert.h"
#include "Usul/System/Clock.h"
#include "Usul/Endian/Endian.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <limits>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderSTL::TriangleReaderSTL ( const std::string &file, Unknown *caller, TriangleDocument *doc ) :
  _file      ( file ),
  _caller    ( caller ),
  _document  ( doc )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderSTL::~TriangleReaderSTL()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the file is ascii.
//
///////////////////////////////////////////////////////////////////////////////

bool TriangleReaderSTL::_isAscii() const
{
  // Get the size of the file.
  Usul::Types::Uint64 size ( Usul::File::size ( _file ) );

  // Open the file in binary.
  std::ifstream in ( _file.c_str(), std::ifstream::in | std::ifstream::binary );

  // Move the cursor so that it's almost to the end.
  in.seekg ( static_cast < std::streamoff > ( ( size > 100 ) ? size - 100 : size * 0.9 ) );

  // Get the remainder of the file as a string.
  std::string file;
  Usul::File::contents ( in, file );

  // Can we find the word "endsolid"?
  return ( std::string::npos != file.find ( "endsolid" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Count the triangles in the file. Note: tried to pass in the already-opened 
//  ifstream object, but while I could easily reset the stream position, I was 
//  unable to reset the state so that "in.eof()" would return false.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int TriangleReaderSTL::_countAcsiiFacets()
{
  // Get the size of the file.
  const Usul::Types::Uint64 fileSize ( Usul::File::size ( _file ) );

  // Open the file.
  std::ifstream in ( _file.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 4181036950: Failed to open ASCII STL file: " + _file );

  // Feedback.
  _document->setStatusBar ( "Counting ASCII triangles..." );

  // Update progress bar every second.
  Usul::Policies::TimeBased elapsed ( 1000 );

  // Make a buffer for the lines.
  const unsigned int bufSize ( 1023 );
  char buffer[bufSize+1];

  // Initialize the count.
  unsigned int count ( 0 );

  // While we're not at the end...
  while ( !in.eof() )
  {
    // Get the type.
    in.getline ( buffer, bufSize );

    // If the type is a facet...
    if ( 0 == ::strcmp ( "endfacet", buffer ) )
    {
      // Increment the count.
      ++count;

      // Show progress.
      _document->setProgressBar ( elapsed(), in, fileSize, _caller );
    }
  }

  // Return the count.
  return count;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderSTL::operator()()
{
  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  // Initialize start time.
  Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

  // Read the data.
  if ( this->_isAscii() )
    this->_readAscii();
  else 
    this->_readBinary();

  // Feedback.
  const double total ( static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 );
  ::printf ( "%8.4f seconds .... Time to read file.\n", total ); ::fflush ( stdout );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the ascii file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderSTL::_readAscii()
{
  // The document is not binary.
  _document->binary ( false );

  // Count the triangles.
  const unsigned int numTriangles ( this->_countAcsiiFacets() );

  // Get the size of the file.
  const Usul::Types::Uint64 fileSize ( Usul::File::size ( _file ) );

  // Open the file.
  std::ifstream in ( _file.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 1022444721: Failed to open ASCII STL file: " + _file );

  // Reserve space in the document.
  _document->setStatusBar ( "Reserving space for new triangles..." );
  _document->reserveTriangles ( _document->numTriangles() + numTriangles );

  // Needed in the loop. Declared here for speed.
  osg::Vec3f v0, v1, v2, n;
  std::string type, temp;
  type.reserve ( 1024 );
  temp.reserve ( 1024 );

  // Feedback.
  _document->setStatusBar ( "Reading ASCII triangle data..." );

  // Update progress bar every second.
  Usul::Policies::TimeBased elapsed ( 1000 );

  // While we're not at the end...
  while ( !in.eof() )
  {
    // Get the type.
    in >> type;

    // If the type is a facet...
    if ( "facet" == type )
    {
      // Read the normal.
      in >> temp >> n[0] >> n[1] >> n[2];
      n.normalize();
    }

    // If the type is a vertex...
    else if ( "vertex" == type )
    {
      // Read all 3 vertices. Note: more than 3 will break this...
      in >> v0[0] >> v0[1] >> v0[2] >> temp;
      in >> v1[0] >> v1[1] >> v1[2] >> temp;
      in >> v2[0] >> v2[1] >> v2[2];
    }

    // If this is the end of the facet...
    else if ( "endfacet" == type )
    {
      // Add the triangle.  Mark as orginal.
      OsgTools::Triangles::Triangle* t ( _document->addTriangle ( v0, v1, v2, n, false, true ) );
      t->original ( true );

      // Show progress.
      _document->setProgressBar ( elapsed(), in, fileSize, _caller );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the binary file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderSTL::_readBinary()
{
  // The document is binary.
  _document->binary ( true );

  // Create a larger buffer.
  const unsigned long int bufSize ( 50 );
  char buffer[bufSize];
  
  // Set the buffer.
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  
  // Open the file in binary.
  in.open ( _file.c_str(), std::ifstream::in | std::ifstream::binary );
  
  // Make sure it's open.
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 3151797946: Failed to open binary STL file: " + _file );

  // Skip the header.
  in.seekg ( 80 );

  // Get the total number of triangles.
  Usul::Types::Uint32 numTriangles ( 0 );
  Usul::IO::Binary::ReadLittleEndian::read ( in, numTriangles );

  // Reserve space in the document.
  _document->setStatusBar ( "Reserving space for new triangles..." );
  _document->reserveTriangles ( _document->numTriangles() + numTriangles );

  // Initialize.
  unsigned int count ( 0 );
  std::ifstream::pos_type position ( in.tellg() );

  // Update progress bar every second.
  Usul::Policies::TimeBased elapsed ( 1000 );

  // Feedback.
  _document->setStatusBar ( "Reading binary triangle data..." );

  // Size of the facet.
  const unsigned int size ( 50 );

  // Struct to hold triangle info
  struct Facet
  {
    osg::Vec3 n, v0, v1, v2;
    char p0, p1;
  };

  // While there are still facets to read...
  while ( count < numTriangles )
  {
    // Read the facet.
    Facet facet;
    in.read( (char*) &facet, size );

    // Swap v0
    Usul::Endian::FromLittleToSystem::convert ( facet.v0[0] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v0[1] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v0[2] );

    // Swap v1
    Usul::Endian::FromLittleToSystem::convert ( facet.v1[0] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v1[1] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v1[2] );

    // Swap v2
    Usul::Endian::FromLittleToSystem::convert ( facet.v2[0] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v2[1] );
    Usul::Endian::FromLittleToSystem::convert ( facet.v2[2] );

    // Swap normal
    Usul::Endian::FromLittleToSystem::convert ( facet.n[0] );
    Usul::Endian::FromLittleToSystem::convert ( facet.n[1] );
    Usul::Endian::FromLittleToSystem::convert ( facet.n[2] );

    // Should be true.
    position += 50;
    USUL_ASSERT ( in.tellg() == position );

    // Add the triangle.  Mark as orginal.
    OsgTools::Triangles::Triangle *t ( _document->addTriangle ( facet.v0, facet.v1, facet.v2, facet.n, false, true ) );
    t->original ( true );

    // Show progress.
    _document->setProgressBar ( elapsed(), count, numTriangles, _caller );
    ++count;
  }
}
