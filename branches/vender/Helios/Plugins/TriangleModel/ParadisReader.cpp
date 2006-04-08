
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ParadisReader.h"

#include "Usul/File/Stats.h"
#include "Usul/Endian/Endian.h"
#include "Usul/IO/Reader.h"

#include <fstream>
#include <algorithm>


#ifdef USUL_BIG_ENDIAN
  const Usul::Types::Uint32 PARADIS = 0x50524453;
  const Usul::Types::Uint32 BIGE = 0x42494745;
  const Usul::Types::Uint32 LITE = 0x4C495445;
  const Usul::Types::Uint32 INDEX = 0x494E4458;
  const Usul::Types::Uint32 PRECIPITATE = 0x50435054;
#elif defined (USUL_LITTLE_ENDIAN)
  const Usul::Types::Uint32 PARADIS = 0x53445250;
  const Usul::Types::Uint32 BIGE = 0x45474942;
  const Usul::Types::Uint32 LITE = 0x4554494C;
  const Usul::Types::Uint32 INDEX = 0x58444E49;
  const Usul::Types::Uint32 PRECIPITATE = 0x54504350;
#endif

const Usul::Types::Uint32 VERSION = 1;

  namespace Detail
  {
    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Functor to swap bytes in containers.
    //
    ///////////////////////////////////////////////////////////////////////////////

    struct SwapBytes
    {
      SwapBytes ( bool littleEndian ) :
      _littleEndian ( littleEndian )
      {
      }

      void operator () ( float &t ) const
      {
        if( _littleEndian )
          Usul::Endian::FromLittleToSystem::convert ( t );
        else
          Usul::Endian::FromBigToSystem::convert ( t );
      }

      void operator () ( Usul::Types::Uint32 &t ) const
      {
        if( _littleEndian )
          Usul::Endian::FromLittleToSystem::convert ( t );
        else
          Usul::Endian::FromBigToSystem::convert ( t );
      }

    private:
      bool _littleEndian;
    };


    ///////////////////////////////////////////////////////////////////////////////
    //
    //  Helper functions for reading in proper endian order.
    //
    ///////////////////////////////////////////////////////////////////////////////

    template < class T > class read
    {
    public:
      static T value ( std::istream &in, bool littleEndian )
      {
        T t ( 0 );

        // If the file data is in little endian...
        if( littleEndian )
        {
          Usul::IO::ReadLittleEndian::read ( in, t );
        }

        // If the file data is in big endian...
        else
        {
          Usul::IO::ReadBigEndian::read ( in, t );
        }

        // Return the value we read.
        return t;
      }

      template < class SizeType > static void sequence ( std::istream &in, SizeType length, T &v, bool littleEndian )
      {
        // Size the sequence.
        v.resize ( static_cast < typename T::size_type > ( length ) );

        // Read the sequence.
        in.read( (char*) &v.front(), length * sizeof ( typename T::value_type ) );

        // Swap the bytes.
        // For some reason, I would get an assert on the following line when swaping the vector of Uint32. TODO: find out why.
        //std::for_each ( v.begin(), v.end(), SwapBytes( littleEndian ) );
        for ( typename T::iterator iter = v.begin(); iter != v.end(); ++iter )
        {
          if( littleEndian )
            Usul::Endian::FromLittleToSystem::convert ( *iter );
          else
            Usul::Endian::FromBigToSystem::convert ( *iter );
        }
      }
    };
  }


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ParadisReader::ParadisReader ( const std::string &file, IUnknown *caller, TriangleDocument *doc ) :
_file ( file, Usul::File::size( file ) ),
_caller ( caller ),
_document ( doc ),
_triangles ( new OsgTools::Triangles::TriangleSet ),
_isLittleEndian ( true ),
_indexRecordOffset ( 0 ),
_precipRecordOffset ( 0 ),
_indexEntries ( ),
_precipitates ( ),
_subsets (),
_shared (),
_indices (),
_counts ()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ParadisReader::~ParadisReader()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear accumulated state.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::clear()
{
  // We do not hand this data to the document, so we can clear it.
  _shared.clear();
  _indices.clear();
  _counts.clear();
  _indexEntries.clear();
  _precipitates.clear();
  _subsets.clear();

  // We hand this to the document, so we cannot clear it.
  _triangles = new OsgTools::Triangles::TriangleSet;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::operator()()
{
  // Clear anything we may have.
  this->clear();

  // Read the file.
  this->_read();

  // Build the triangles
  this->_build();

  // Add the triangle set.
  _document->addTriangleSet ( _triangles );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_read( )
{
  // Open a stream with a large buffer.
  const unsigned long int bufSize ( 4095 );
  char buffer[bufSize+1];
  std::ifstream in;
  in.rdbuf()->pubsetbuf ( buffer, bufSize );
  in.open ( _file.first.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2868855740: Failed to open file: " + _file.first );

  // Read the header.
  this->_readHeader ( in );

  // Read the index record
  this->_readIndexRecord ( in );

  // Read the precipitates
  this->_readPrecipitates ( in );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the header.  Throws if finds bad data.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_readHeader ( std::istream& in )
{
  // Feedback.
  _document->setStatusBar ( "Reading the header..." );

  // Read the Record Type.
  {
    const unsigned int size ( sizeof ( Usul::Types::Uint32 ) );
    char id[size];
    ::memset ( id, 0, size );

    in.read ( id, size );

    if ( 0 != ::memcmp ( id, &PARADIS, size ) )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1020312934: Unknown header ID in Paradis file", id, 
          ";\n Expected: ", PARADIS, 
          ";\n Found: ", id, 
          ";\n File: ", _file.first );
    }
  }

  // Read the endian order.
  {
    const unsigned int size ( sizeof ( Usul::Types::Uint32 ) );
    char id[size];
    ::memset ( id, 0, size );

    in.read ( id, size );

    // Little endian.
    if( 0 == ::memcmp ( id, &LITE, size ) )
    {
      _isLittleEndian = true;
    }

    // Big endian
    else if ( 0 == ::memcmp ( id, &BIGE, size ) )
    {
      _isLittleEndian = false;
    }

    // Error
    else
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Unknown endian type Paradis file",
          ";\n Expected: ", LITE, 
          " or ", BIGE, 
          ";\n Found: ", id );
    }
  }

  // Read the length.
  Usul::Types::Uint32 length ( Detail::read< Usul::Types::Uint32 >::value( in, _isLittleEndian ) );

  if ( 84 != length )
  {
    Usul::Exceptions::Thrower<std::runtime_error> ( "Incorrect length in Paradis file.  Expected length 80, found:", length );
  }

  // Read index record offset.
  _indexRecordOffset = Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian );

  // Read first precipitate record offset.
  _precipRecordOffset = Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian );

  Usul::Types::Uint64 metaDataOffset = Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian );

  // Read the version.
  {
    Usul::Types::Uint32 version ( Detail::read < Usul::Types::Uint32 >::value
                                  ( in, _isLittleEndian ) );

    if ( version != VERSION )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Unknown version of Paradis file",
          ";\n Expected: ", VERSION, 
          ";\n Found: ", version );
    }
  }

  // Read the size of float.
  Usul::Types::Uint32 floatSize ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

  if( 4 != floatSize )
  {
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Floats size of ", floatSize, " are not handled.  \nExpected size of 4." );
  }

  // Read the size of int.
  Usul::Types::Uint32 intSize ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

  if( 4 != intSize )
  {
    Usul::Exceptions::Thrower<std::runtime_error> 
      ( "Ints size of ", intSize, " are not handled.  \nExpected size of 4." );
  }

  // Read the simulation limits.
  {
    const unsigned int size ( sizeof ( Usul::Types::Float64 ) * 3 );
    Usul::Types::Float64 simMin[3];
    Usul::Types::Float64 simMax[3];

    in.read ( (char*) &simMin, size );
    in.read ( (char*) &simMax, size );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the index record.  Throws if finds bad data.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_readIndexRecord ( std::istream& in )
{
  // Feedback.
  _document->setStatusBar ( "Reading the index record..." );

  in.seekg( _indexRecordOffset );

  // Read the Record Type.
  {
    const unsigned int size ( sizeof ( Usul::Types::Uint32 ) );
    char id[size];
    ::memset ( id, 0, size );

    in.read ( id, size );

    if ( 0 != ::memcmp ( id, &INDEX, size ) )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Error 1020312934: Unknown index record ID in Paradis file", id, 
          ";\n Expected: ", INDEX, 
          ";\n Found: ", id, 
          ";\n File: ", _file.first );
    }
  }

  // Read the length.
  Usul::Types::Uint32 length ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

  if( ( length - 4 ) % 20 != 0 )
  {
    Usul::Exceptions::Thrower<std::runtime_error> (
      "Incorrect length in index record of Paradis file:", length );
  }
  
  // Read the number of precipitates.
  Usul::Types::Uint32 numPrecips ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

  if( ( length - 4 ) / 20 != numPrecips )
  {
    Usul::Exceptions::Thrower<std::runtime_error> (
      "Invalid number of precipitates in index record of Paradis file.",
      "\nExpected: ", numPrecips, "\nFound: ", ( ( length - 4 ) / 20 ) );
  }

  _indexEntries.reserve ( numPrecips );
  for ( unsigned int i = 0; i < numPrecips; ++i )
  {
    Usul::Types::Uint32 id       ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint32 numVerts ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint32 numTris  ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint64 offset   ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );

    IndexEntry entry;
    entry.id = id;
    entry.numVerts = numVerts;
    entry.numTris = numTris;
    entry.offset = offset;

    _indexEntries.push_back( entry );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the precipitates.  Throws if finds bad data.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_readPrecipitates ( std::istream& in )
{
  for( unsigned int i = 0; i < _indexEntries.size(); ++i )
  {
      // Feedback.
    std::ostringstream os;
    os << "Reading precipitate " << i << "...";
    _document->setStatusBar ( os.str() );

    IndexEntry entry = _indexEntries.at(i);
    Usul::Types::Uint64 offset ( entry.offset );

    in.seekg( offset );

    // Read the Record Type.
    const unsigned int size32 ( sizeof ( Usul::Types::Uint32 ) );
    char type[size32];
    ::memset ( type, 0, size32 );

    in.read ( type, size32 );

    if ( 0 != ::memcmp ( type, &PRECIPITATE, size32 ) )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
        ( "Unknown precipitate record ID in Paradis file", type, 
          ";\n Expected: ", PRECIPITATE, 
          ";\n Found: ", type, 
          ";\n File: ", _file.first );
    }

    // Read the length.
    Usul::Types::Uint32 length ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

    // Read the id.
    Usul::Types::Uint32 id ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

    if( id != entry.id )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
          ( "Precipitate ids in Paradis file do not match",
            ";\n Expected: ", entry.id, 
            ";\n Found: ", id, 
            ";\n File: ", _file.first );
    }

    // Get the offsets.
    Usul::Types::Uint64 verticesOffset    ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint64 trianglesOffset   ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint64 normalsOffset     ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint64 metadataOffset    ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );
    Usul::Types::Uint64 endOfRecordOffset ( Detail::read< Usul::Types::Uint64 >::value ( in, _isLittleEndian ) );

    // Precipitate Record
    PrecipitateRecord record;

    // Seek to the start of vertices.
    in.seekg( offset + verticesOffset );

    // Get the number of vertices.
    record.numVerts = Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian );

    if( record.numVerts != entry.numVerts )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
          ( "Number of vertices in Paradis file do not match for precipitate", id,
            ";\n Expected: ", entry.numVerts, 
            ";\n Found: ", record.numVerts, 
            ";\n File: ", _file.first );
    }

    // Get the vertex information.
    Detail::read< FloatVector >::sequence ( in, record.numVerts * 3, record.vertices, _isLittleEndian );
    //in.read( (char*) &vertices.front(), vertices.size() * sizeof( float ) );

    // Seek to the start of triangles.
    in.seekg( offset + trianglesOffset );

    // Get the number of triangles.
    record.numTris = Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian );

    if( record.numTris != entry.numTris )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
          ( "Number of triangles in Paradis file do not match for precipitate", id,
            ";\n Expected: ", entry.numTris, 
            ";\n Found: ", record.numTris, 
            ";\n File: ", _file.first );
    }

    // Get the triangle information.
    Detail::read< UintVector >::sequence ( in, record.numTris * 3, record.triangles, _isLittleEndian );
    //in.read( (char*) &triangles.front(), triangles.size() * size32 );

    // Seek to the start of normals.
    in.seekg( offset + normalsOffset );

    // Get the number of normals.
    Usul::Types::Uint32 numberNormals ( Detail::read< Usul::Types::Uint32 >::value ( in, _isLittleEndian ) );

    if( numberNormals != entry.numTris || record.numTris != numberNormals )
    {
      Usul::Exceptions::Thrower<std::runtime_error> 
          ( "Number of normals in Paradis file do not match for precipitate", id,
            ";\n Expected: ", entry.numTris, 
            ";\n Found: ", numberNormals, 
            ";\n File: ", _file.first );
    }

    // Get the normal information.
    Detail::read< FloatVector >::sequence ( in, numberNormals * 3, record.normals, _isLittleEndian );
    //in.read( (char*) &normals.front(), normals.size() * sizeof( float ) );

    _precipitates.push_back ( record );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_build( )
{
  this->_buildVerticesNormalsTriangles();

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

  // Create the blocks for the precipitates.
  this->_buildBlocks();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Make one list of vertices, normals, and triangles.  Retain the connectivity.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildVerticesNormalsTriangles()
{
  // Feedback.
  _document->setStatusBar ( "Creating vectors of vertices, normals, and triangles..." );

  Normals &normals ( *_triangles->normalsT() );
  Vertices &vertices ( *_triangles->vertices() );

  // Go through the precipitates.
  for( unsigned int i = 0; i < _precipitates.size(); ++i )
  {
    PrecipitateRecord record ( _precipitates.at( i ) );

    // Vector of connected triangles.
    Connected connected;

    const unsigned int previousSize ( vertices.size() );

    vertices.reserve( previousSize + record.numVerts );

    // Add the vertices.
    for( FloatVector::iterator iter = record.vertices.begin(); iter != record.vertices.end(); iter += 3 )
    {
      osg::Vec3 vertex ( *iter, *(iter + 1), *(iter + 2 ) );
      _triangles->updateBounds ( vertex );
      vertices.push_back( vertex );
    }

    normals.reserve( normals.size() + record.numTris );

    // Add the normals.
    for( FloatVector::iterator iter = record.normals.begin(); iter != record.normals.end(); iter += 3 )
    {
      normals.push_back( osg::Vec3 ( *iter, *(iter + 1), *(iter + 2 ) ) );
    }

    // Add the triangles.
    for( UintVector::iterator iter = record.triangles.begin(); iter != record.triangles.end(); iter += 3 )
    {
      unsigned int v0 ( *iter );
      unsigned int v1 ( *(iter + 1 ) );
      unsigned int v2 ( *(iter + 2 ) );
      
      unsigned int number ( _indices.size() );

      v0 += previousSize;
      v1 += previousSize;
      v2 += previousSize;

      _indices.push_back ( Indices::value_type ( v0, v1, v2 ) );

      connected.push_back( number );

    }

    _subsets.push_back ( connected );
  }

}



///////////////////////////////////////////////////////////////////////////////
//
//  Build the container of vertex-usage counts.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildVertexUsageCounts()
{
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

  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the vector of shared vertices.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildSharedVerticesVector()
{
  // Get the number of vertices.
  Vertices &vertices ( *_triangles->vertices() );
  const unsigned int numVertices ( vertices.size() );

  // Should be true.
  USUL_ASSERT ( _counts.size() == numVertices );

  // Make space.
  _shared.reserve ( numVertices );

  // Feedback.
  _document->setStatusBar ( "Building vector of shared vertices..." );

  // Loop through the vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    // Declare new shared-vertex and tell it how many triangles to reserve.
    // This assumes all vertices in the file are unique.
    _shared.push_back ( _triangles->newSharedVertex ( i, _counts[i] ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the map of shared vertices.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildSharedVerticesMap()
{
  // Get the number of vertices.
  Vertices &vertices ( *_triangles->vertices() );
  const unsigned int numVertices ( vertices.size() );

  // Shortcut to the map.
  typedef OsgTools::Triangles::TriangleSet::SharedVertices VertexMap;
  VertexMap &shared ( _triangles->sharedVertices() );

  // Feedback.
  _document->setStatusBar ( "Building map of shared vertices..." );

  // Loop through the vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    // Add it to the map.
    shared.insert ( VertexMap::value_type ( vertices[i], _shared[i].get() ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangles.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildTriangles()
{
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

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build per-vertex normals if we need to.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildNormalsV()
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
//  Build the blocks for each connected set of triangles.
//
///////////////////////////////////////////////////////////////////////////////

void ParadisReader::_buildBlocks()
{
  _triangles->createSubsets ( _subsets, _caller );
}
