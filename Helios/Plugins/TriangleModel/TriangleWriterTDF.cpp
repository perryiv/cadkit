
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  TDF writer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleWriterTDF.h"
#include "TriangleConstants.h"

#include "OsgTools/Triangles/Triangle.h"

#include "Usul/File/Temp.h"
#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Policies/Update.h"
#include "Usul/Strings/Format.h"
#include "Usul/IO/BinaryWriter.h"

#include <cstdlib>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleWriterTDF::TriangleWriterTDF ( const std::string &file, Unknown *caller, const TriangleDocument *doc ) :
  _file     ( file ),
  _caller   ( caller ),
  _document ( const_cast < TriangleDocument * > ( doc ) )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleWriterTDF::~TriangleWriterTDF()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  To shorten the lines.
//
///////////////////////////////////////////////////////////////////////////////

#define WRITE_SCALAR(exp) Usul::IO::Binary::WriteLittleEndian::write  ( file.stream(), (exp) )
#define WRITE_VEC3(exp)   Usul::IO::Binary::WriteLittleEndian::write3 ( file.stream(), (exp) )
#define WRITE_VEC4(exp)   Usul::IO::Binary::WriteLittleEndian::write4 ( file.stream(), (exp) )
#define WRITE_STRING(exp) file.stream().write ( (exp).c_str(), (exp).length() )


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleWriterTDF::operator()()
{
  // Sizes.
  const FileFormat::Size::Record sequenceSize ( sizeof ( FileFormat::Size::Sequence ) );
  const FileFormat::Size::Scalar scalarSize   ( sizeof ( FileFormat::Size::Scalar   ) );

  // Progress.
  Usul::Policies::TimeBased update ( 1000 );
  Usul::Interfaces::IProgressBar::ShowHide scope ( Usul::Resources::progressBar() );

  // Make a temporary file
  Usul::File::Temp file ( Usul::File::Temp::BINARY );

  // Initialize the progress counters.
  unsigned int count ( 0 );
  const unsigned int total ( _document->vertices()->size() + 
                             _document->triangleSet()->sharedVertices().size() + 
                             _document->normalsV()->size() + 
                             _document->normalsT()->size() + 
                             _document->triangles().size() );

  // Write the header.
  {
    file.stream().write( (char*) &FileFormat::Header::ID, sizeof(FileFormat::Header::ID)  );
    file.stream().write( (char*) &FileFormat::Header::ENDIAN_LITTLE, sizeof(FileFormat::Header::ENDIAN_LITTLE) );

    WRITE_SCALAR ( FileFormat::Header::VERSION_MAJOR );
    WRITE_SCALAR ( FileFormat::Header::VERSION_MINOR );
  }

  // Write some meaningful meta data.
  {
    std::ostringstream info;
    info << "<Format>Triangle Document File (TDF)</Format>"
         << "<Version>" << FileFormat::Header::VERSION_MAJOR << '.' << FileFormat::Header::VERSION_MINOR << "</Version>"
         << "<Endian>Little</Endian>";
    const FileFormat::Size::Record recordSize ( info.str().length() );
    WRITE_SCALAR ( FileFormat::Record::META_DATA );
    WRITE_SCALAR ( recordSize );
    WRITE_STRING ( info.str() );
  }

  // Write the bounding box.
  {
    const osg::BoundingBox bbox ( _document->getBoundingBox() );
    const FileFormat::Size::Scalar floatSize  ( sizeof ( float ) );
    const FileFormat::Size::Record recordSize ( scalarSize + floatSize * 6 );

    WRITE_SCALAR ( FileFormat::Record::BOUNDING_BOX );
    WRITE_SCALAR ( recordSize );
    WRITE_SCALAR ( floatSize  );
    WRITE_SCALAR ( bbox.xMin() );
    WRITE_SCALAR ( bbox.yMin() );
    WRITE_SCALAR ( bbox.zMin() );
    WRITE_SCALAR ( bbox.xMax() );
    WRITE_SCALAR ( bbox.yMax() );
    WRITE_SCALAR ( bbox.zMax() );
  }

  // Write the vertices.
  {
    const osg::Vec3Array &vertices ( *_document->vertices() );
    const FileFormat::Size::Sequence numVertices ( vertices.size() );
    const FileFormat::Size::Scalar floatSize  ( sizeof ( float ) );
    const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numVertices * floatSize * 3 );

    _document->setStatusBar ( "Writing vertices..." );

    WRITE_SCALAR ( FileFormat::Record::VERTICES );
    WRITE_SCALAR ( recordSize  );
    WRITE_SCALAR ( numVertices );
    WRITE_SCALAR ( floatSize   );

    for ( osg::Vec3Array::size_type i = 0; i < numVertices; ++i )
    {
      WRITE_VEC3 ( vertices[i] );
      _document->setProgressBar ( update(), count++, total, _caller );
    }
  }

  // Write the sorted vertex indices.
  {
    typedef OsgTools::Triangles::TriangleSet TriangleSet;
    typedef TriangleSet::SharedVertices SharedVertices;
    const TriangleSet &ts ( *_document->triangleSet() );
    const SharedVertices &sv ( ts.sharedVertices() );
    const FileFormat::Size::Sequence numVertices ( sv.size() );
    const FileFormat::Size::Scalar indexSize ( sizeof ( unsigned int ) );
    const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numVertices * indexSize );

    _document->setStatusBar ( "Writing sorted vertex indices..." );

    WRITE_SCALAR ( FileFormat::Record::SORTED_VERTICES );
    WRITE_SCALAR ( recordSize  );
    WRITE_SCALAR ( numVertices );
    WRITE_SCALAR ( indexSize   );

    for ( SharedVertices::const_iterator i = sv.begin(); i != sv.end(); ++i )
    {
      WRITE_SCALAR ( static_cast < unsigned int > ( i->second->index() ) );
      _document->setProgressBar ( update(), count++, total, _caller );
    }
  }

  // Write the triangle normals.
  {
    const osg::Vec3Array &normals ( *_document->normalsT() );
    const FileFormat::Size::Sequence numNormals ( normals.size() );
    const FileFormat::Size::Scalar floatSize  ( sizeof ( float ) );
    const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numNormals * floatSize * 3 );

    _document->setStatusBar ( "Writing per-triangle normal vectors..." );

    WRITE_SCALAR ( FileFormat::Record::TRIANGLE_NORMALS );
    WRITE_SCALAR ( recordSize );
    WRITE_SCALAR ( numNormals );
    WRITE_SCALAR ( floatSize  );

    for ( osg::Vec3Array::size_type i = 0; i < numNormals; ++i )
    {
      WRITE_VEC3 ( normals[i] );
      _document->setProgressBar ( update(), count++, total, _caller );
    }
  }

  // Write the vertex normals.
  {
    const osg::Vec3Array &normals ( *_document->normalsV() );
    const FileFormat::Size::Sequence numNormals ( normals.size() );
    if ( numNormals > 0 )
    {
      _document->setStatusBar ( "Writing per-vertex normal vectors..." );

      const FileFormat::Size::Scalar floatSize  ( sizeof ( float ) );
      const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numNormals * floatSize * 3 );

      WRITE_SCALAR ( FileFormat::Record::VERTEX_NORMALS );
      WRITE_SCALAR ( recordSize );
      WRITE_SCALAR ( numNormals );
      WRITE_SCALAR ( floatSize  );

      for ( osg::Vec3Array::size_type i = 0; i < numNormals; ++i )
      {
        WRITE_VEC3 ( normals[i] );
        _document->setProgressBar ( update(), count++, total, _caller );
      }
    }
  }

  // Write the vertex colors.
  {
    const osg::Vec4Array &colors ( *_document->getColorsV ( false ) );
    const FileFormat::Size::Sequence numColors ( colors.size() );
    if ( numColors > 0 )
    {
      _document->setStatusBar ( "Writing per-vertex color vectors..." );

      const FileFormat::Size::Scalar floatSize  ( sizeof ( float ) );
      const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numColors * floatSize * 4 );

      WRITE_SCALAR ( FileFormat::Record::VERTEX_COLORS );
      WRITE_SCALAR ( recordSize );
      WRITE_SCALAR ( numColors );
      WRITE_SCALAR ( floatSize  );

      for ( osg::Vec4Array::size_type i = 0; i < numColors; ++i )
      {
        WRITE_VEC4 ( colors[i] );
        _document->setProgressBar ( update(), count++, total, _caller );
      }
    }
  }

  // Write the triangles, which is just each vertex's index.
  {
    typedef TriangleDocument::TriangleVector TriangleVector;
    const TriangleVector &triangles ( _document->triangles() );
    const FileFormat::Size::Sequence numTriangles ( triangles.size() );
    const FileFormat::Size::Scalar indexSize  ( sizeof ( TriangleVector::value_type::element_type::IndexType ) );
    const FileFormat::Size::Record recordSize ( sequenceSize + scalarSize + numTriangles * indexSize * 3 );

    _document->setStatusBar ( "Writing triangles..." );

    WRITE_SCALAR ( FileFormat::Record::TRIANGLES );
    WRITE_SCALAR ( recordSize   );
    WRITE_SCALAR ( numTriangles );
    WRITE_SCALAR ( indexSize    );

    for ( TriangleVector::size_type i = 0; i < numTriangles; ++i )
    {
      const OsgTools::Triangles::Triangle &triangle ( *(triangles[i]) );
      WRITE_SCALAR ( triangle.vertex0()->index() );
      WRITE_SCALAR ( triangle.vertex1()->index() );
      WRITE_SCALAR ( triangle.vertex2()->index() );
      _document->setProgressBar ( update(), count++, total, _caller );
    }
  }

  // TODO... Write loops?

  // Rename temporary file to final filename
  try
  {
    file.rename ( _file );
  }
  // Copy if there was an error.
  catch ( ... )
  {
    const std::string command ( Usul::Strings::format ( "cp ", file.name(), " ", _file ) );
    std::cout << command << std::endl;
    system ( command.c_str() );
  }
}
