
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  STL writer class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleWriterSTL.h"

#include "Usul/File/Temp.h"

#include "OsgTools/IO/STLWrite.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Policies/Update.h"

#include <fstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleWriterSTL::TriangleWriterSTL ( const std::string &file, bool binary, Unknown *caller, const TriangleDocument *doc ) :
  _file     ( file ),
  _caller   ( caller ),
  _document ( const_cast < TriangleDocument * > ( doc ) ),
  _binary   ( binary )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleWriterSTL::~TriangleWriterSTL()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Template function to write the triangles.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class WriterType_ > void write ( TriangleDocument &document, WriterType_ &writer, Usul::Interfaces::IUnknown *caller )
  {
    Usul::Policies::TimeBased update ( 1000 );
    Usul::Interfaces::IProgressBar::ValidQueryPtr progressBar ( Usul::Resources::progressBar() );
    Usul::Interfaces::IProgressBar::ShowHide scope ( progressBar.get() );

    // Loop through the triangles.
    unsigned int numTriangles ( document.numTriangles() );
    for ( unsigned int i = 0; i < numTriangles; ++i )
    {
      // Get the vertices and normal.
      const osg::Vec3f &v0 = document.vertex0 ( i );
      const osg::Vec3f &v1 = document.vertex1 ( i );
      const osg::Vec3f &v2 = document.vertex2 ( i );
      const osg::Vec3f &n  = document.getTriangleNormal  ( i );

      document.setProgressBar ( update(), i, numTriangles, caller );

      // Write the data. Note the order!
      writer ( n, v0, v1, v2 );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleWriterSTL::operator()()
{
  // Should we write binary?
  if ( _binary )
  {
    // Make a temporary file
    Usul::File::Temp file ( Usul::File::Temp::BINARY );

    // Get number of triangles.
    unsigned int numTriangles ( _document->numTriangles() );

    // Create a header.
    std::string header ( "solid " + _file );
    header.resize ( 80, ' ' );
    file.stream().write ( header.c_str(), header.length() );
    Usul::IO::Binary::WriteLittleEndian::write ( file.stream(), numTriangles );

    // Write the triangles in binary.
    typedef OsgTools::IO::BinaryWriter < osg::Vec3f > BinaryWriter;
    BinaryWriter writer (file.stream() );
    Detail::write ( *_document, writer, _caller );

    // Rename temporary file to final filename
    file.rename ( _file );
  }

  // Otherwise, ascii.
  else
  {
    // Make a temporary file
    Usul::File::Temp file ( Usul::File::Temp::ASCII );

    // Start the solid.
    file.stream() << "solid " << _file << std::endl;

    // Write the triangles in ascii.
    typedef OsgTools::IO::AsciiWriter < osg::Vec3f > AsciiWriter;
    AsciiWriter writer (file.stream() );
    Detail::write ( *_document, writer, _caller );

    // End of the solid.
    file.stream() << "endsolid" << std::endl;

    // Rename temporary file to final filename
    file.rename ( _file );
  }
}
