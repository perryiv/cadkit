
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterR3D.h"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"

#include "Usul/Endian/Endian.h"

#include <fstream>
#include <stdexcept>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::ReaderWriterR3D() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::~ReaderWriterR3D()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterR3D::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "r3d" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterR3D::className()
{
  return "R3D Reader/Writer";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::ReadResult ReaderWriterR3D::readNode ( const std::string &file, const Options *options )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterR3D::ReadResult &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterR3D::ReadResult ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterR3D::ReadResult ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group * ReaderWriterR3D::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize internal data structures.
//
///////////////////////////////////////////////////////////////////////////////

void  ReaderWriterR3D::_init()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::ReadResult ReaderWriterR3D::_read ( const std::string &filename, const Options *options )
{
  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Make sure the internal data members are initialized.
  this->_init();

  // Open the binary file.
  std::ifstream in ( filename.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2260991012: Failed to open binary file: " + filename );

  // Parse the file.
  this->_parse ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Check to see if the next word read is the same as the given word.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterR3D::_check ( std::istream &in, const std::string &s ) const
{
  std::string a, b ( s );
  in >> a;
  std::transform ( a.begin(), a.end(), a.begin(), ::tolower );
  std::transform ( b.begin(), b.end(), b.begin(), ::tolower );
  if ( a != b )
  {
    std::ostringstream out;
    out << "Error 2040984431: Invalid word '" << a << "' found at beginning of R3D file.";
      throw std::runtime_error ( out.str() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip the next word.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterR3D::_skipWord ( std::istream &in ) const
{
  std::string a;
  in >> a;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Skip the next line.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterR3D::_skipLine ( std::istream &in ) const
{
  const unsigned int size ( 1023 );
  char buf[size+1];
  in.getline ( buf, size );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterR3D::_parse ( std::istream &in )
{
  // Check the header.
  this->_check ( in, "#" );
  this->_check ( in, "vtk" );
  this->_check ( in, "DataFile" );
  this->_check ( in, "Version" );
  this->_skipLine ( in );
  this->_skipLine ( in );
  this->_check ( in, "binary" );
  this->_check ( in, "dataset" );
  this->_check ( in, "polydata" );
  this->_check ( in, "points" );

  // Read the number of points.
  unsigned int num ( 0 );
  in >> num;

  // Should be floating point data.
  this->_check ( in, "float" );
  this->_skipLine ( in );

  // Start reading the points.
  for ( unsigned int i = 0; i < num; ++i )
  {
    Usul::Types::Float32 x, y, z;
    in.read ( USUL_UNSAFE_CAST ( char *, &x ), sizeof ( Usul::Types::Float32 ) );
    in.read ( USUL_UNSAFE_CAST ( char *, &y ), sizeof ( Usul::Types::Float32 ) );
    in.read ( USUL_UNSAFE_CAST ( char *, &z ), sizeof ( Usul::Types::Float32 ) );
    Usul::Endian::reverseBytes ( x );
    Usul::Endian::reverseBytes ( y );
    Usul::Endian::reverseBytes ( z );
    double dummy = x;
    dummy = y;
    dummy = z;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterR3D> g_ReaderWriter_R3D_proxy;
