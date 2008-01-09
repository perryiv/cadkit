
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_IO_STL_WRITE_H__
#define __OSG_TOOLS_IO_STL_WRITE_H__

#include "Usul/MPL/SameType.h"
#include "Usul/Types/Types.h"
#include "Usul/IO/BinaryWriter.h"

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Resources/ProgressBar.h"
#include "Usul/Policies/Update.h"
#include "Usul/Exceptions/Thrower.h"

#include <iostream>

namespace OsgTools {
namespace IO {

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in Ascii
//
///////////////////////////////////////////////////////////////////////////////

template < class Vertex >
struct AsciiWriter
{
  AsciiWriter ( std::ostream& out ) : _out ( out ) { }
  void operator() ( const Vertex &normal, const Vertex &v1, const Vertex &v2, const Vertex &v3 )
  {
    _out << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << " " << std::endl;
    _out << "  outer loop " << std::endl;
    _out << "    vertex "   << v1[0] << " " << v1[1] << " " << v1[2] << " " << std::endl;
    _out << "    vertex "   << v2[0] << " " << v2[1] << " " << v2[2] << " " << std::endl;
    _out << "    vertex "   << v3[0] << " " << v3[1] << " " << v3[2] << " " << std::endl;
    _out << "  endloop"     << std::endl;
    _out << "endfacet"      << std::endl;
  }
private:
  std::ostream &_out;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in binary. STL is little-endian. 
//  See: http://www.transcendata.com/cadfix/support/changeNotesDX.htm
//
///////////////////////////////////////////////////////////////////////////////

template < class Vertex >
struct BinaryWriter
{
  BinaryWriter ( std::ostream& out ) : _out ( out ) { }
  void operator() ( const Vertex &normal, const Vertex &v1, const Vertex &v2, const Vertex &v3 )
  {
    // Current algorithm only works on 32-bit machines.
    USUL_ASSERT_SAME_TYPE ( Usul::Types::Float32, float );

    // Write the normal and vertices.
    Usul::IO::Binary::WriteLittleEndian::write3 ( _out, normal );
    Usul::IO::Binary::WriteLittleEndian::write3 ( _out, v1 );
    Usul::IO::Binary::WriteLittleEndian::write3 ( _out, v2 );
    Usul::IO::Binary::WriteLittleEndian::write3 ( _out, v3 );

    // Write spacers to bring record size to 50 bytes.
    _out.write ( "\0", 1 );
    _out.write ( "\0", 1 );
  }
private:
  std::ostream &_out;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Struct for writing a STL file
//
///////////////////////////////////////////////////////////////////////////////

template < class Writer > 
struct WriteSTLFile
{
  WriteSTLFile() { }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Write out the STL file using vertex sequence
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class VertexSequence, class NormalSequence >
  static void write ( std::ostream& out, const VertexSequence& vertices, const NormalSequence& normals )
  {
    // Throw if the number of normals is wrong.
    if( normals.size() != vertices.size() / 3 )
    {
      Usul::Exceptions::Thrower< std::runtime_error >
        ( "Normals size is ", normals.size(), ".  Expecting size ", vertices.size() / 3, "\n" );
    }

    // Throw if the number of Vertices is wrong.
    if( vertices.size() % 3 != 0 )
    {
      Usul::Exceptions::Thrower< std::runtime_error >
        ( "Vertices size is ", vertices.size(), ".  Expecting size ", vertices.size() / 3, "\n" );
    }

    //Create the writer
    Writer writer ( out );

    Usul::Policies::TimeBased update ( 1000 );
    Usul::Interfaces::IProgressBar::ValidQueryPtr progressBar ( Usul::Resources::progressBar() );

    Usul::Interfaces::IProgressBar::ShowHide scope( progressBar.get() );

    //Loop through the vertices three at a time
    for( unsigned int i = 0; i < vertices.size(); i += 3 )
    {
      writer ( normals.at( i / 3 ), vertices.at( i ), vertices.at( i + 1 ), vertices.at ( i + 2 ) );

      if( update() )
      {
        progressBar->updateProgressBar( i / vertices.size() );
      }

    }
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Write out the STL file using triangle sequence
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class TriangleSequence >
  static void write ( std::ostream& out, const TriangleSequence& triangles )
  {
    typedef typename TriangleSequence::value_type Triangle;
    typedef typename TriangleSequence::iterator TriangleItr;
    typedef typename Triangle::VertexType Vertex;

    //Create the writer
    Writer writer( out );

    //Loop through the triangles
    for( TriangleItr i = triangles.begin(); i != triangles.end(); ++ i )
    {
      Vertex normal( i->normal() );
      Vertex v1 ( i->value1() );
      Vertex v2 ( i->value2() );
      Vertex v3 ( i->value3() );

      writer ( normal, v1, v2, v3 );
    }
  }

  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Write out the STL file using vertex sequence
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class VertexSequence, class NormalSequence >
  void operator () ( std::ostream& out, const VertexSequence& vertices, const NormalSequence& normals )
  {
    write ( out, vertices, normals );
  }


  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Write out the STL file using triangle sequence
  //
  ///////////////////////////////////////////////////////////////////////////////

  template < class TriangleSequence >
  void operator () ( std::ostream& out, const TriangleSequence& triangles )
  {
    write ( out, triangles );
  }
};


}
}


#endif //__OSG_TOOLS_IO_STL_WRITE_H__

