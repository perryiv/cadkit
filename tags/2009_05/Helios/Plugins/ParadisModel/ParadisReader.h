
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

#include "ParadisDocument.h"
#include "FastLoadTriangleSet.h"

#include "Usul/Types/Types.h"

#include "osg/PrimitiveSet"

#include <iosfwd>
#include <vector>

class ParadisReader
{
public:
  // Typedefs.
  typedef ParadisDocument::ValidRefPtr ParadisDocumentPtr;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < std::string, unsigned long > FileInfo;
  typedef FastLoadTriangleSet TriangleSet;

  typedef osg::DrawElementsUInt Elements;
  typedef osg::ref_ptr < Elements > ElementsPtr;

  typedef TriangleSet::VerticesPtr VerticesPtr;
  typedef TriangleSet::NormalsPtr NormalsPtr;

  typedef TriangleSet::VerticesPtr::element_type Vertices;
  typedef TriangleSet::NormalsPtr::element_type Normals;

  typedef std::vector< float > FloatVector;
  typedef std::vector< Usul::Types::Uint32 > UintVector;

  // Construction/destruction.
  ParadisReader ( const std::string &file, IUnknown *caller, ParadisDocument *doc );
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
  ParadisDocumentPtr _document;
  TriangleSet::ValidRefPtr _triangles;
  bool _isLittleEndian;
  Usul::Types::Uint64 _indexRecordOffset;
  Usul::Types::Uint64 _precipRecordOffset;
  std::vector < IndexEntry > _indexEntries;
  std::vector < PrecipitateRecord > _precipitates;

};


#endif // __PARADIS_READER_H__

