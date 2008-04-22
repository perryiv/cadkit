
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterSTL.h"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"

#include "OsgTools/Visitor.h"
#include "OsgTools/Statistics.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/File/Stats.h"
#include "Usul/File/Temp.h"
#include "Usul/File/Contents.h"
#include "Usul/IO/Writer.h"

#include <sstream>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <locale>

#include "Functors.h"
#include "PrintVisitor.h"

#define USE_FAST_PATH // Need per-vertex normals for osg::Geometry's "fast path".


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::ReaderWriterSTL() : BaseClass(),
  _polygons(),
  _binary ( true )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::~ReaderWriterSTL()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterSTL::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "stl" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterSTL::className()
{
  return "STL Reader/Writer";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::Result ReaderWriterSTL::readNode ( const std::string &file, const Options *options )
{
  try
  {
    // Make sure we handle files with this extension.
    if ( !this->acceptsExtension ( osgDB::getFileExtension ( file ) ) )
      return ReadResult::FILE_NOT_HANDLED;

    // Make sure the internal data members are initialized.
    this->_init();

    Update progress ( 0x0 );

    // Read the scene
    this->_read ( file, progress );

    // Build the scene.
    osg::ref_ptr<osg::Group> root ( this->_build() );

    // Initialized again to free accumulated data.
    this->_init();

    // Return the scene
    return root.release();
  }

  // Catch known exceptions.
  catch ( const ReaderWriterSTL::Result &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterSTL::Result ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterSTL::Result ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterSTL::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // A single geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  root->addChild ( geode.get() );

  // Make triangles.
  Polygons::const_iterator i = _polygons.find ( 3 );
  if ( _polygons.end() != i )
  {
    // Shortcuts to the vertices and normals.
    const Vertices &v = i->second.first;
    const Normals  &n = i->second.second;

#ifdef USE_FAST_PATH // Need per-vertex normals for osg::Geometry's "fast path".
    assert ( n.size() == v.size() );
#else
    assert ( n.size() * 3 == v.size() );
#endif

    // Make vertices and normals for the geometry.
    osg::ref_ptr<osg::Vec3Array> vertices ( new osg::Vec3Array ( v.begin(), v.end() ) );
    osg::ref_ptr<osg::Vec3Array> normals  ( new osg::Vec3Array ( n.begin(), n.end() ) );

    // Make geometry and add to geode.
    osg::ref_ptr<osg::Geometry> geom  ( new osg::Geometry );
    geode->addDrawable ( geom.get() );  

    // Set vertices and normals.
    geom->setVertexArray ( vertices.get() );
    geom->setNormalArray ( normals.get() );
    geom->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

#ifdef USE_FAST_PATH // Need per-vertex normals for osg::Geometry's "fast path".
    geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    geom->setUseVertexBufferObjects ( true );
#else
    geom->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
#endif

    // Interpret every three osg::Vec3 as a triangle.
    geom->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLES, 0, vertices->size() ) );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize internal data structures.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_init()
{
  _polygons.clear();
  this->binary ( true );
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the file is ascii.
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterSTL::_isAscii ( const std::string &filename ) const
{
  // Get the size of the file.
  unsigned int size ( Usul::File::size ( filename ) );

  // Open the file in binary.
  std::ifstream in ( filename.c_str(), std::ifstream::in | std::ifstream::binary );

  // Move the cursor so that it's almost to the end.
  in.seekg ( ( size > 100 ) ? size - 100 : size * 0.9 );

  // Get the remainder of the file as a string.
  std::string file;
  Usul::File::contents ( in, file );

  // Can we find the word "endsolid"?
  return ( std::string::npos != file.find ( "endsolid" ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the binary file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_parseBinaryFile ( std::istream &in, const Update &progress )
{
  // Skip header.
  in.seekg ( 80 );

  const unsigned int size ( 512 );
  char buf[size];

  int numFacets = 0;
  
  in.read(buf, 4);

  ::memcpy(&numFacets, buf, 4);

  const unsigned int totalFacets ( numFacets );
  unsigned int facetsReadSoFar ( 0 );
  double lastTime ( ::clock() );

  while ( numFacets > 0 )
  {
    float f[3];

    in.read(buf, 50);
    ::memcpy(f, buf, 12); // copy first 12 bytes for normal
    osg::Vec3 n ( f[0], f[1], f[2] );
    _polygons[3].second.push_back ( n );

#ifdef USE_FAST_PATH // Need per-vertex normals for osg::Geometry's "fast path".
    _polygons[3].second.push_back ( n );
    _polygons[3].second.push_back ( n );
#endif

    ::memcpy(f, buf+12, 12);  // copy first vertex
    osg::Vec3 v0 ( f[0], f[1], f[2] );
    _polygons[3].first.push_back ( v0 );

    ::memcpy(f, buf+24, 12);  // copy second vertex
    osg::Vec3 v1 ( f[0], f[1], f[2] );
    _polygons[3].first.push_back ( v1 );

    ::memcpy(f, buf+36, 12);  // copy third vertex
    osg::Vec3 v2 ( f[0], f[1], f[2] );
    _polygons[3].first.push_back ( v2 );
    
    --numFacets;

    facetsReadSoFar++;

    if ( ( ::clock() - lastTime ) > 500 )
    {
      progress ( ( (float ) facetsReadSoFar / totalFacets ) * 100 );
      lastTime = ::clock();
    }
  }
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Parse the ascii file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_parseAsciiFile ( std::istream &in, unsigned int filesize, const Update &progress )
{
  unsigned int bytesReadSoFar ( 0 );
  const unsigned int size ( 512 );
  char buf[size];
  Vertices vertices;
  Normals normals;

  double lastTime ( ::clock() );

  while( !in.eof() )
  {
    in.getline(buf, size - 1);

    bytesReadSoFar += ::strlen ( buf );

    std::istringstream in ( buf );

    std::string type;
    in >> type;

    if(type == "solid")
    {
    }
    else if (type == "facet")
    {
      std::string normal;
      in >> normal;
      osg::Vec3 n;
      in >> n[0] >> n[1] >> n[2];
      normals.push_back ( n );

#ifdef USE_FAST_PATH // Need per-vertex normals for osg::Geometry's "fast path".
      normals.push_back ( n );
      normals.push_back ( n );
#endif
    }
    else if (type == "outer")
    {
    }
    else if (type == "vertex")
    {
      osg::Vec3 v;
      in >> v[0] >> v[1] >> v[2];
      vertices.push_back ( v );
    }
    else if (type == "endloop")
    {
      // Make sure we have one of these.
      _polygons[vertices.size()];

      // Now get short-cuts.
      Vertices &v = _polygons[vertices.size()].first;
      Normals  &n = _polygons[vertices.size()].second;

      // Append the new vertices and normals.
      v.insert ( v.end(), vertices.begin(), vertices.end() );
      n.insert ( n.end(), normals.begin(),  normals.end()  );

      // Clear the temporary lists.
      vertices.clear();
      normals.clear();
    }
    else if (type == "endfacet")
    {
    }
    else if (type == "endsolid")
    {
    }

    if ( ( ::clock() - lastTime ) > 500 )
    {
      progress ( ( ( float ) bytesReadSoFar / filesize ) * 100 ); 
      lastTime = ::clock();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_read ( const std::string &filename, const Update& progress )
{
  // See if the file is ascii.
  if ( this->_isAscii ( filename ) )
  {
    this->binary ( false );
    std::ifstream in ( filename.c_str() );
    if ( !in.is_open() )
      throw std::runtime_error ( "Error 2346450991: Failed to open ascii file: " + filename );
    this->_parseAsciiFile ( in, Usul::File::size( filename ), progress );
  }

  // Otherwise, read the binary file.
  else
  {
    this->binary ( true );
    std::ifstream in ( filename.c_str(), std::ifstream::in | std::ifstream::binary );
    if ( !in.is_open() )
      throw std::runtime_error ( "Error 3162884175: Failed to open binary file: " + filename );
    this->_parseBinaryFile ( in, progress );
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write the file
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::WriteResult ReaderWriterSTL::writeNode ( const osg::Node& node, const std::string& fileName, const Options* options )
{ 
  std::string ext = osgDB::getFileExtension ( fileName );
  if ( !this->acceptsExtension ( ext ) )
    return WriteResult::FILE_NOT_HANDLED;

  // Get options in lower-case.
  std::string chunk ( ( options ) ? options->getOptionString() : "ascii" );
  std::transform ( chunk.begin(), chunk.end(), chunk.begin(), ::tolower );

  // Write appropriate format.
  if ( chunk == "binary" )
    return _writeBinary( node, fileName );
  else if ( chunk == "ascii" )
    return _writeAscii( node, fileName );

  // Incorrect options.
  std::ostringstream out;
  out << "Error 2815147711: Invalid options string: " << chunk << "', expected 'binary' or 'ascii'";
  throw std::invalid_argument ( out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write an ascii file. See: http://www.ennex.com/~fabbers/StL.asp
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::WriteResult ReaderWriterSTL::_writeAscii  ( const osg::Node& node, const std::string& name )
{
  // Make a temporary file name.
  Usul::File::Temp file ( Usul::File::Temp::ASCII );

  file.stream() << "solid " << node.getName() << std::endl;

  // Functor to write out facets.
  AsciiWriter writer ( file.stream() );

  // Visitor to write out file
  osg::ref_ptr<osg::NodeVisitor> printVisitor ( new PrintVisitor<AsciiWriter> ( writer ) );

  // This visitor does not modify the scene.
  osg::Node &n = const_cast< osg::Node& > ( node );
  n.accept( *printVisitor );

  file.stream() << "endsolid " << std::endl;

  // If we get this far then rename the temporary file.
  file.rename ( name );

  return WriteResult::FILE_SAVED;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write an binary file. See: http://www.ennex.com/~fabbers/StL.asp
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::WriteResult ReaderWriterSTL::_writeBinary ( const osg::Node& node, const std::string& name )
{
  // Make a temporary file name.
  Usul::File::Temp file ( Usul::File::Temp::BINARY );

  // Count number of facets.
  OsgTools::Statistics sceneStats;
  sceneStats.greedy ( true );
  sceneStats.accumulate ( &node );
  unsigned int numFacets ( sceneStats.count ( OsgTools::Statistics::TRIANGLES ) );

  // Create a header.
  std::string header ( "solid " + node.getName() );
  header.resize ( 80, ' ' );
  file.stream().write ( header.c_str(), header.length() );
  Usul::IO::WriteLittleEndian::write ( file.stream(), numFacets );

  // Functor to write out facets
  BinaryWriter writer ( file.stream() );

  // Visitor to write out file
  osg::ref_ptr<osg::NodeVisitor> printVisitor ( new PrintVisitor<BinaryWriter> ( writer ) );
  osg::Node &n = const_cast < osg::Node& > ( node );
  n.accept ( *printVisitor );

  // If we get this far then rename the temporary file.
  file.rename ( name );

  return WriteResult::FILE_SAVED;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set binary flag.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::binary ( bool b )
{
  _binary = b;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we binary?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterSTL::binary() const
{
  return _binary;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterSTL> g_ReaderWriter_STL_proxy;
