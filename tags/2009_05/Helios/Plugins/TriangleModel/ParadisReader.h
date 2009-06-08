
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Paradis reader class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __PARADIS_READER_H__
#define __PARADIS_READER_H__

#include "TriangleDocument.h"

#include "Usul/Types/Types.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include <iosfwd>
#include <vector>

class ParadisReader
{
public:
  // Typedefs.
  typedef TriangleDocument::ValidRefPtr TriangleDocumentPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < std::string, Usul::Types::Uint64 > FileInfo;
  typedef OsgTools::Triangles::TriangleSet TriangleSet;
  typedef TriangleSet::TriangleVector TriangleVector;
  typedef TriangleSet::VerticesPtr::element_type Vertices;
  typedef TriangleSet::NormalsPtr::element_type Normals;
  typedef TriangleSet::Connected Connected;
  typedef TriangleSet::Subsets Subsets;
  typedef std::vector< float > FloatVector;
  typedef std::vector< Usul::Types::Uint32 > UintVector;
  typedef OsgTools::Triangles::Triangle Triangle;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef std::vector < SharedVertex::ValidRefPtr > SharedVertices;
  typedef std::vector < unsigned int > Counts;
  typedef std::vector < Usul::Math::Vec3ui > Indices;

  // Construction/destruction.
  ParadisReader ( const std::string &file, IUnknown *caller, TriangleDocument *doc );
  ~ParadisReader();

  // Clear accumulated state.
  void                clear();

  // Read the file.
  void                operator()();

protected:

  void _read( );

  void _readHeader ( std::istream& in );
  void _readIndexRecord ( std::istream& in );
  void _readPrecipitates ( std::istream& in );

  void _build( );
  void _buildVerticesNormalsTriangles();
  void _buildVertexUsageCounts();
  void _buildNormalsV();
  void _buildSharedVerticesVector();
  void _buildSharedVerticesMap();
  void _buildTriangles();
  void _buildBlocks();

private:
  struct IndexEntry
  {
    Usul::Types::Uint32 id;
    Usul::Types::Uint32 numVerts;
    Usul::Types::Uint32 numTris;
    Usul::Types::Uint64 offset;
  };

  struct PrecipitateRecord
  {
    Usul::Types::Uint32 numVerts;
    Usul::Types::Uint32 numTris;
    FloatVector vertices;
    UintVector  triangles;
    FloatVector normals;
  };

  FileInfo _file;
  IUnknown::RefPtr _caller;
  TriangleDocumentPtr _document;
  TriangleSet::ValidRefPtr _triangles;
  bool _isLittleEndian;
  Usul::Types::Uint64 _indexRecordOffset;
  Usul::Types::Uint64 _precipRecordOffset;
  std::vector < IndexEntry > _indexEntries;
  std::vector < PrecipitateRecord > _precipitates;
  Subsets _subsets;
  SharedVertices _shared;
  Indices _indices;
  Counts _counts;
};


#endif // __PARADIS_READER_H__

