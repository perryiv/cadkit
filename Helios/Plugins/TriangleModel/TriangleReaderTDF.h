
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

#ifndef _TRIANGLE_MODEL_TDF_READER_CLASS_H_
#define _TRIANGLE_MODEL_TDF_READER_CLASS_H_

#include "TriangleDocument.h"
#include "TriangleConstants.h"

#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include <string>
#include <iosfwd>
#include <vector>


class TriangleReaderTDF
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
  typedef TriangleSet::ColorsPtr::element_type Colors;
  typedef std::vector < Usul::Math::Vec3ui > Indices;
  typedef std::vector < unsigned int > SortOrder;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef OsgTools::Triangles::Triangle Triangle;
  typedef std::vector < SharedVertex::ValidRefPtr > SharedVertices;
  typedef std::vector < unsigned int > Counts;
  typedef std::pair < unsigned int, unsigned int > Progress;

  // Construction/destruction.
  TriangleReaderTDF ( const std::string &file, IUnknown *caller, TriangleDocument *doc );
  ~TriangleReaderTDF();

  // Clear accumulated state.
  void                clear();

  // Read the file.
  void                operator()();

protected:

  void                _addTriangles();

  void                _build();
  void                _buildNormalsV();
  void                _buildSharedVerticesVector();
  void                _buildSharedVerticesMap();
  void                _buildTriangles();
  void                _buildVertexUsageCounts();

  void                _canBuild() const;

  void                _incrementProgress ( bool state );

  void                _read();
  void                _readHeader    ( std::ifstream &in );
  void                _readRecord    ( std::ifstream &in );
  void                _readRecords   ( std::ifstream &in );
  void                _readColorsV   ( std::ifstream &in );
  void                _readNormalsT  ( std::ifstream &in );
  void                _readNormalsV  ( std::ifstream &in );
  void                _readTriangles ( std::ifstream &in );
  void                _readVertices  ( std::ifstream &in );
  void                _readSortOrder ( std::ifstream &in );
  void                _readBounds    ( std::ifstream &in );

  void                _skipRecord ( std::ifstream &in, FileFormat::Record::Type type, FileFormat::Size::Record size );

  void                _updateDocument();

private:

  // No copying.
  TriangleReaderTDF ( const TriangleReaderTDF & );
  TriangleReaderTDF &operator = ( const TriangleReaderTDF & );

  FileInfo _file;
  IUnknown::RefPtr _caller;
  TriangleDocumentPtr _document;
  SharedVertices _shared;
  Indices _indices;
  Counts _counts;
  SortOrder _sortOrder;
  TriangleSet::ValidRefPtr _triangles;
  Progress _progress;
};


#endif // _TRIANGLE_MODEL_TDF_READER_CLASS_H_
