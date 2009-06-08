
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  OFF reader class.
//
///////////////////////////////////////////////////////////////////////////////

#include "TriangleReaderOFF.h"

#include "Usul/Policies/Update.h"
#include "Usul/IO/BinaryReader.h"
#include "Usul/Types/Types.h"
#include "Usul/Errors/Assert.h"
#include "Usul/System/Clock.h"

#include <iostream>
#include <fstream>
#include <stdexcept>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

TriangleReaderOFF::TriangleReaderOFF ( const std::string &file, Unknown *caller, TriangleDocument *doc ) :
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

TriangleReaderOFF::~TriangleReaderOFF()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the file.
//
///////////////////////////////////////////////////////////////////////////////

void TriangleReaderOFF::operator()()
{
  // Set initial progress and range.
  _document->setProgressBar ( true, 0, 100, _caller );

  // Initialize start time.
  Usul::Types::Uint64 start ( Usul::System::Clock::milliseconds() );

  // Open File.
  std::ifstream fin ( _file.c_str () );

  std::string word ( "" );
  fin >> word;

  // Make sure it's an OFF file.
  if ( "OFF" != word )
    throw std::runtime_error ( "Error 3526352675: Invalid OFF file." );

  // Get the number of vertices and triangles.
  unsigned int numVertices ( 0 ), numTriangles ( 0 ), numEdges ( 0 );
  fin >> numVertices >> numTriangles >> numEdges;

  // Check what we got.
  if ( 0 == numVertices || 0 == numTriangles )
    throw std::runtime_error ( "Error : Number of vertices and triangles must be greater than 0 ." );

  // A temporary vector for the vertices.
  std::vector < osg::Vec3 > vertices;

  // Reserve enough room.
  vertices.reserve ( numVertices );
  _document->reserveTriangles ( _document->numTriangles() + numTriangles );

  // Read the vertices.
  for ( unsigned int i = 0; i < numVertices; ++i )
  {
    osg::Vec3 v;
    fin >> v[0] >> v[1] >> v[2];
    vertices.push_back ( v );
  }

  // Read the triangles.
  for ( unsigned int i = 0; i < numTriangles; ++ i )
  {
    unsigned int faces ( 0 ), i0 ( 0 ), i1 ( 0 ), i2 ( 0 );
    fin >> faces >> i0 >> i1 >> i2;

    if ( 3 == faces )
    {
      osg::Vec3 v0 ( vertices.at ( i0 ) );
      osg::Vec3 v1 ( vertices.at ( i1 ) );
      osg::Vec3 v2 ( vertices.at ( i2 ) );

      osg::Vec3 a ( v2 - v1 );
      osg::Vec3 b ( v0 - v1 );

      osg::Vec3 n ( a ^ b );
      n.normalize();

      // Add the triangle. Mark as orginal.
      OsgTools::Triangles::Triangle* t ( _document->addTriangle ( v0, v1, v2, n, false, true ) );
      t->original ( true );
    }
  }

  // Feedback.
  const double total ( static_cast < double > ( Usul::System::Clock::milliseconds() - start ) * 0.001 );
  ::printf ( "%8.4f seconds .... Time to read file.\n", total ); ::fflush ( stdout );
}
