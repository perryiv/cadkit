
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry Miller
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterSPDAT.h"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "Usul/Exceptions/Thrower.h"

#include "OsgTools/Mesh.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSPDAT::ReaderWriterSPDAT() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSPDAT::~ReaderWriterSPDAT()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterSPDAT::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "spdat" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterSPDAT::className()
{
  return "SPDAT Reader";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSPDAT::Result ReaderWriterSPDAT::readNode ( const std::string &file, const Options *options )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterSPDAT::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterSPDAT::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterSPDAT::Result ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSPDAT::Result ReaderWriterSPDAT::_read ( const std::string &filename, const Options * )
{
  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Open the file.
  std::ifstream in ( filename.c_str(), std::ios::in );
  if ( !in )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1389143263, failed to open.",
        "\n\tFile: ", filename );
  }

  // Declare the mesh.
  OsgTools::Mesh mesh;

  // Read the number of points.
  unsigned int rows    ( 0 );
  unsigned int columns ( 0 );
  in >> rows >> columns;
  if ( rows < 2 )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1208199594, Invalid number of rows in file, need at least two.",
        "\n\tFile: ", filename,
        "\n\tRows: ", rows );
  }
  if ( columns < 2 )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1226441327, Invalid number of columns in file, need at least two.",
        "\n\tFile:    ", filename,
        "\n\tColumns: ", columns );
  }

  // The points and normals.
  typedef OsgTools::Mesh::Vectors Vectors;
  Vectors points, normals;
  points.resize  ( rows * columns );
  normals.resize ( rows * columns );

  // Declare here for loop speed.
  unsigned int row, column;

  // Read the points.
  for ( Vectors::size_type j = 0; j < points.size(); ++j )
  {
    // Make sure there are more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3070575716, there should be ", points.size(), " data points.",
          "\n\tFile: ", filename );
    }

    // Discard for now...
    in >> row >> column;

    // Set this element.
    in >> points[j][0] >> points[j][1] >> points[j][2];

    // Read the rest of the line, discarding additional dimensions.
    while ( '\n' != in.peek() )
      in.get();
  }

  // Read the normals.
  for ( Vectors::size_type i = 0; i < normals.size(); ++i )
  {
    // Make sure there are more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3418646572, there should be ", normals.size(), " normals.",
          "\n\tFile: ", filename );
    }

    // Discard for now...
    in >> row >> column;

    // Set this element.
    in >> normals[i][0] >> normals[i][1] >> normals[i][2];
  }

  // Assign the members.
  mesh.size ( rows, columns );
  mesh.points ( points );
  mesh.normals ( normals );

  // Normalize the normals.
  mesh.normalize();

  // Return the new scene.
  return mesh();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterSPDAT> g_ReaderWriter_SPDAT_proxy;
