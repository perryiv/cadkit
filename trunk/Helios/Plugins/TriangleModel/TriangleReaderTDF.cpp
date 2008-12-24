
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TDF reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleReaderTDF.h"
#include "TriangleHelpersTDF.h"

#include "Usul/File/Stats.h"
#include "Usul/Policies/Update.h"
#include "Usul/IO/BinaryReader.h"
#include "Usul/Exceptions/Thrower.h"
#include "Usul/Resources/TextWindow.h"
#include "Usul/Math/Vector3.h"
#include "Usul/MPL/SameType.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <cstdlib>


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

TriangleReaderTDF::TriangleReaderTDF ( const std::string &file, Usul::Interfaces::IUnknown *caller, TriangleDocument *doc ) :
  _file      ( file, Usul::File::size ( file ) ),
  _caller    ( caller ),
  _document  ( doc ),
  _shared    (),
  _indices   (),
  _counts    (),
  _sortOrder (),
  _triangles ( new OsgTools::Triangles::TriangleSet ),
  _progress  ( 0, 1 )
{
  // Needs to be true.
  USUL_ASSERT_SAME_TYPE ( float, osg::Vec3f::value_type );
  USUL_ASSERT_SAME_TYPE ( osg::Vec3f, Vertices::value_type );
  USUL_ASSERT_SAME_TYPE ( osg::Vec3f, Normals::value_type );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderTDF::~TriangleReaderTDF()
{
  // Need to do this because of circular references. Otherwise there are leaks.
  this->clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::operator()()
{
  // Clear accumulated state.
  this->clear();

  // Read the file.
  this->_read();

  // If the document already has triangles...
  if ( _document->numTriangles() > 0 )
  {
    // Add the new ones.
    this->_addTriangles();
  }

  // Otherwise...
  {
    // Build the triangles.
    this->_build();

    // Update the document's data.
    this->_updateDocument();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize. Note: we do not clear the reference-counted objects because 
//  we hand those to the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::clear()
{
  // We do not hand this data to the document, so we can clear it.
  _shared.clear();
  _indices.clear();
  _counts.clear();
  _sortOrder.clear();
  _progress = Progress ( 0, 1 );

  // We hand this to the document, so we cannot clear it.
  _triangles = new OsgTools::Triangles::TriangleSet;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_read()
{
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  in.open ( _file.first.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2868855740: Failed to open TDF file: " + _file.first );

  // Read the header.
  this->_readHeader ( in );

  // Read all the records.
  this->_readRecords ( in );

  // Set the progress numbers.
  _progress.first = 0;
  _progress.second = 2 * _triangles->vertices()->size() + 2 * _indices.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the header.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readHeader ( std::ifstream &in )
{
  // Read the header id.
  {
    // Size of the string includes the terminating null character.
    const unsigned int size ( sizeof ( FileFormat::Header::ID ) );
    char id[size];
    ::memset ( id, 0, size );

    // There is no terminating null character in the file.
    in.read ( id, size );    
    if ( 0 != ::memcmp ( id, FileFormat::Header::ID, size ) )
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1020312934: Unknown header ID in TDF file", id, 
          ";\n Expected: ", FileFormat::Header::ID, 
          ";\n Found: ", id, 
          ";\n File: ", _file.first );
  }

  // Read the endian order.
  {
    // Size of the string includes the terminating null character.
    const unsigned int sizeEndian ( sizeof ( FileFormat::Header::ENDIAN_LITTLE ) );
    char order[sizeEndian];
    ::memset ( order, 0, sizeEndian );

    // There is no terminating null character in the file.
    in.read ( order, sizeEndian );

    // First, see if it is big endian.
    if ( 0 == ::memcmp ( order, FileFormat::Header::ENDIAN_BIG, sizeEndian ) )
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 2620365053: This implementation does not support Big Endian formating",
          ";\nFile: ", _file.first );

    // When we get to here, it better be little endian.
    if ( 0 != ::memcmp ( order, FileFormat::Header::ENDIAN_LITTLE, sizeEndian ) )
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1020312934: Unknown endian order in TDF file", 
          ";\n Expected: ", FileFormat::Header::ENDIAN_LITTLE,
          ";\n Found: ", order, 
          ";\n File: ", _file.first );
  }

  // Read the version.
  {
    FileFormat::Header::VersionNumberType major = 0;
    FileFormat::Header::VersionNumberType minor = 0;
    Usul::IO::Binary::ReadLittleEndian::read ( in, major );
    Usul::IO::Binary::ReadLittleEndian::read ( in, minor );
    if ( FileFormat::Header::VERSION_MAJOR != major || FileFormat::Header::VERSION_MINOR != minor )
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1810383068: Unsupported TDF version '", major, '.', minor, "' in file: ", _file.first );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read all records.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readRecords ( std::ifstream &in )
{
  // Progress.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );

  // Feedback.
  _document->setStatusBar ( "Reading binary records..." );

  // Read all the records.
  while ( EOF != in.peek() )
  {
    // Read a single record.
    this->_readRecord ( in );

    // Feedback.
    _document->setProgressBar ( update(), in, _file.second, _caller );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the record.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readRecord ( std::ifstream &in )
{
  // Make sure there is more.
  Detail::checkEndOfFile ( in, _file );

  // Read the record type and size.
  const FileFormat::Record::Type type ( Detail::Read<FileFormat::Record::Type>::value ( in, "record type" ) );
  const FileFormat::Size::Record size ( Detail::Read<FileFormat::Size::Record>::value ( in, "record size" ) );

  // Make sure enough bytes exist in the file.
  Detail::isFileBigEnough ( in, size, _file );

  // Process the record type.
  switch ( type )
  {
    case FileFormat::Record::VERTICES:
    {
      this->_readVertices ( in );
      break;
    }
    case FileFormat::Record::SORTED_VERTICES:
    {
      this->_readSortOrder ( in );
      break;
    }
    case FileFormat::Record::TRIANGLE_NORMALS:
    {
      this->_readNormalsT ( in );
      break;
    }
    case FileFormat::Record::VERTEX_NORMALS:
    {
      this->_readNormalsV ( in );
      break;
    }
    case FileFormat::Record::TRIANGLES:
    {
      this->_readTriangles ( in );
      break;
    }
    case FileFormat::Record::BOUNDING_BOX:
    {
      this->_readBounds ( in );
      break;
    }
    case FileFormat::Record::VERTEX_COLORS:
    {
      this->_readColorsV ( in );
      break;
    }
    default:
    {
      this->_skipRecord ( in, type, size );
      break;
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vertices.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readVertices ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numVertices ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of vertices"   ) );
  const FileFormat::Size::Scalar   floatSize   ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "floating point bytes" ) );

  // Make sure we support the floating-point size.
  Detail::checkSizes ( in, "floating point", sizeof ( Vertices::value_type ), 3 * floatSize, _file );

  // Read the vertices.
  Vertices &vertices ( *_triangles->vertices() );
  Detail::Read<Vertices>::sequence ( in, "vertices", numVertices, vertices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the sorted vertices.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readSortOrder ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numVertices ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of sorted vertex indices" ) );
  const FileFormat::Size::Scalar   indexSize   ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "vertex index bytes"              ) );

  // Make sure we support the floating-point size.
  Detail::checkSizes ( in, "vertex index", sizeof ( SortOrder::value_type ), indexSize, _file );

  // Read the indices.
  Detail::Read<SortOrder>::sequence ( in, "sorted vertex indices", numVertices, _sortOrder );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the triangle normals.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readNormalsT ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numNormals ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of normals"    ) );
  const FileFormat::Size::Scalar   floatSize  ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "floating point bytes" ) );

  // Make sure we support the floating-point size.
  Detail::checkSizes ( in, "floating point", sizeof ( Normals::value_type ), 3 * floatSize, _file );

  // Read the per triangle normals.
  Normals &normals ( *_triangles->normalsT() );
  Detail::Read<Normals>::sequence ( in, "triangle normals", numNormals, normals );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vertex normals.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readNormalsV ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numNormals ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of normals"    ) );
  const FileFormat::Size::Scalar   floatSize  ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "floating point bytes" ) );

  // Make sure we support the floating-point size.
  Detail::checkSizes ( in, "floating point", sizeof ( Normals::value_type ), 3 * floatSize, _file );

  // Read the per vertex normals.
  Normals &normals ( *_triangles->normalsV() );
  Detail::Read<Normals>::sequence ( in, "triangle normals", numNormals, normals );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vertex colors.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readColorsV ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numColors ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of colors"     ) );
  const FileFormat::Size::Scalar   floatSize ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "floating point bytes" ) );

  // Make sure we support the floating-point size.
  Detail::checkSizes ( in, "floating point", sizeof ( Colors::value_type ), 4 * floatSize, _file );

  // Make the colors.
  Colors &colors ( *_triangles->getColorsV ( false ) );
  colors.resize ( numColors );

  // Read the per vertex colors.
  Detail::Read<Colors>::sequence ( in, "triangle colors", numColors, colors );

  // The colors are not dirty.
  _triangles->dirtyColorsV ( false );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readTriangles ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Sequence numTriangles ( Detail::Read<FileFormat::Size::Sequence>::value ( in, "number of triangles"  ) );
  const FileFormat::Size::Scalar   indexSize    ( Detail::Read<FileFormat::Size::Scalar>::value   ( in, "triangle index bytes" ) );

  // Make sure we support the index size.
  Detail::checkSizes ( in, "index", sizeof ( Indices::value_type ), 3 * indexSize, _file );

  // Read the indices.
  Detail::Read<Indices>::sequence ( in, "triangle indices", numTriangles, _indices );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_readBounds ( std::ifstream &in )
{
  // Get the integer data.
  const FileFormat::Size::Scalar floatSize ( Detail::Read<FileFormat::Size::Scalar>::value ( in, "floating point bytes" ) );

  // Make sure we support the floating-point size.
  typedef float FloatType;
  Detail::checkSizes ( in, "floating point", sizeof ( FloatType ), floatSize, _file );

  // Read the values.
  typedef Detail::Read<FloatType> Read;
  const FloatType xMin ( Read::value ( in, "bounding box x min", false ) );
  const FloatType yMin ( Read::value ( in, "bounding box y min", false ) );
  const FloatType zMin ( Read::value ( in, "bounding box z min", false ) );
  const FloatType xMax ( Read::value ( in, "bounding box x max", false ) );
  const FloatType yMax ( Read::value ( in, "bounding box y max", false ) );
  const FloatType zMax ( Read::value ( in, "bounding box z max", false ) );

  // Set the bounding box.
  _triangles->updateBounds ( osg::Vec3f ( xMin, yMin, zMin ) );
  _triangles->updateBounds ( osg::Vec3f ( xMax, yMax, zMax ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip the record.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_skipRecord ( std::ifstream &in, FileFormat::Record::Type type, FileFormat::Size::Record size )
{
  std::cout << "Warning 3560521088: Skipping unrecognized record type " << type 
            << " of length " << size << " in file: " << _file .first
            << Usul::Resources::TextWindow::endl;

  // We do this in case the record length is greater than seekg can move at once.
  while ( size )
  {
    const std::ifstream::off_type offset ( static_cast < std::ifstream::off_type > ( size ) );
    in.seekg ( offset, std::ios::cur );
    size -= offset;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_addTriangles()
{
  USUL_ASSERT ( 0 ); // TODO
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_build()
{
  // Make sure we have the data we need.
  this->_canBuild();

  // Build the container of vertex-usage counts.
  this->_buildVertexUsageCounts();

  // Build the vector of shared vertices first.
  this->_buildSharedVerticesVector();

  // Build the map of shared vertices first.
  this->_buildSharedVerticesMap();

  // Build the triangles.
  this->_buildTriangles();

  // Build per-vertex normals if we need to.
  this->_buildNormalsV();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if we have enough to build.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_canBuild() const
{
  const unsigned int numIndices  ( _indices.size() );
  const unsigned int numVertices ( _triangles->vertices()->size() );
  const unsigned int numNormalsT ( _triangles->normalsT()->size() );

  if ( 0 == numIndices || 0 == numVertices || 0 == numNormalsT )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1197949366: cannot build triangles with zero-length sequences: ",
        "triangle indices = ", numIndices, 
        ", vertices = ", numVertices, 
        ", per-triangle normal vectors = ", numNormalsT );
  }

  if ( numIndices != numNormalsT )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1832429564: number of triangle indices ", numIndices, 
        " is not equal to the number of per-triangle normal vectors ", numNormalsT );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the container of vertex-usage counts.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_buildVertexUsageCounts()
{
  // Progress.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );

  // Make space.
  const unsigned int numVertices ( _triangles->vertices()->size() );
  _counts.resize ( numVertices, 0 );

  // Feedback.
  _document->setStatusBar ( "Counting vertex usage..." );

  // Loop through the triangle indices.
  const unsigned int numTriangles ( _indices.size() );
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Get the index.
    const Indices::value_type &index ( _indices[i] );

    // Increment the counts.
    _counts.at ( index[0] ) += 1;
    _counts.at ( index[1] ) += 1;
    _counts.at ( index[2] ) += 1;

    // Progress.
    this->_incrementProgress ( update() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of shared vertices.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_buildSharedVerticesVector()
{
  // Get the number of vertices.
  Vertices &vertices ( *_triangles->vertices() );
  const unsigned int numVertices ( vertices.size() );

  // Should be true.
  USUL_ASSERT ( _counts.size() == numVertices );

  // Make space.
  _shared.reserve ( numVertices );

  // Progress.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );

  // Feedback.
  _document->setStatusBar ( "Building vector of shared vertices..." );

  // Loop through the vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    // Declare new shared-vertex and tell it how many triangles to reserve.
    // This assumes all vertices in the file are unique.
    _shared.push_back ( _triangles->newSharedVertex ( i, _counts[i] ) );

    // Progress.
    this->_incrementProgress ( update() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the map of shared vertices.
//  See http://gcc.gnu.org/onlinedocs/libstdc++/23_containers/howto.html#4
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_buildSharedVerticesMap()
{
  // Get the number of vertices.
  Vertices &vertices ( *_triangles->vertices() );
  const unsigned int numVertices ( vertices.size() );

  // Use this switch to turn on/off the use of a sorted vector.
  const bool useSortOrder ( numVertices == _sortOrder.size() );

  // Shortcut to the map.
  typedef OsgTools::Triangles::TriangleSet::SharedVertices VertexMap;
  VertexMap &shared ( _triangles->sharedVertices() );

  // Progress.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );

  // Feedback.
  _document->setStatusBar ( "Building map of shared vertices..." );

  // Use the sequence of sorted indices.
  if ( useSortOrder )
  {
    // Loop through the vertices.
    for ( unsigned int i = 0; i < numVertices; ++i )
    {
      // Get the proper index.
      const unsigned int index ( _sortOrder[i] );

      // Add it to the map.
      shared.insert ( shared.end(), VertexMap::value_type ( vertices.at ( index ), _shared.at ( index ).get() ) );

      // Progress.
      this->_incrementProgress ( update() );
    }

    // If we don't get as many as we are supposed to, do it again the long way.
    if ( shared.size() != numVertices )
    {
      USUL_ASSERT ( 0 ); // Heads up.
      _sortOrder.clear();
      this->_buildSharedVerticesMap();
    }
  }

  // Otherwise, insert without hinting.
  else
  {
    // Loop through the vertices.
    for ( unsigned int i = 0; i < numVertices; ++i )
    {
      // Add it to the map.
      shared.insert ( VertexMap::value_type ( vertices[i], _shared[i].get() ) );

      // Progress.
      this->_incrementProgress ( update() );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_buildTriangles()
{
  // Progress.
  Usul::Policies::TimeBased update ( Detail::_milliseconds );

  // Shortcut to the triangles.
  TriangleVector &triangles ( _triangles->triangles() );

  // Make space.
  const unsigned int numTriangles ( _indices.size() );
  triangles.reserve ( numTriangles );

  // Feedback.
  _document->setStatusBar ( "Building triangles..." );

  // Loop through the triangles.
  for ( unsigned int i = 0; i < numTriangles; ++i )
  {
    // Get the index.
    const Indices::value_type &index ( _indices[i] );

    // Get the shared vertices.
    SharedVertex::ValidRefPtr &sv0 ( _shared.at ( index[0] ) );
    SharedVertex::ValidRefPtr &sv1 ( _shared.at ( index[1] ) );
    SharedVertex::ValidRefPtr &sv2 ( _shared.at ( index[2] ) );

    // Append new triangle to the list.
    Triangle::ValidRefPtr t ( _triangles->newTriangle ( sv0, sv1, sv2, i ) );
    triangles.push_back ( t.get() );

    // This is an "original" triangle.
    t->original ( true );

    // Should be true because of the way we constructed them.
    USUL_ASSERT ( sv0->capacity() >= sv0->numTriangles() );
    USUL_ASSERT ( sv1->capacity() >= sv1->numTriangles() );
    USUL_ASSERT ( sv2->capacity() >= sv2->numTriangles() );

    // Progress.
    this->_incrementProgress ( update() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build per-vertex normals if we need to.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_buildNormalsV()
{
  // If we read them from the file then return.
  Vertices &vertices ( *_triangles->vertices() );
  Normals &normals   ( *_triangles->normalsV() );
  if ( normals.size() == vertices.size() )
    return;

  // Should be true.
  USUL_ASSERT ( _shared.size() == vertices.size() );

  // Make room.
  const unsigned int numVertices ( _shared.size() );
  normals.resize ( numVertices );

  // Loop through the shared vertices and update the normal.
  for ( unsigned int i = 0; i < numVertices; ++i )
    normals[i] = _triangles->averageNormal ( _shared[i] );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the document.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_updateDocument()
{
  _document->setStatusBar ( "Updating document with new triangle set..." );
  _document->addTriangleSet ( _triangles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the progress bar.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderTDF::_incrementProgress ( bool state )
{
  unsigned int &numerator   ( _progress.first  );
  unsigned int &denominator ( _progress.second );
  _document->setProgressBar ( state, numerator, denominator, _caller );
  ++numerator;
  USUL_ASSERT ( numerator <= denominator );
}
