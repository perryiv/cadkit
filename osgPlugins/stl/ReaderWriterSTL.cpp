
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
#include "Usul/Adaptors/MemberFunction.h"

#include <sstream>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <locale>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::ReaderWriterSTL() :
  _polygons()
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
    return this->_read ( file, options );
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

osg::Group * ReaderWriterSTL::_build() const
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
    assert ( n.size() * 3 == v.size() );

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

void  ReaderWriterSTL::_init()
{
  _polygons.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the file is ascii.
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterSTL::_isAscii ( const std::string &filename ) const
{
  // Open the file in binary.
  std::ifstream in ( filename.c_str(), std::ios::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 3535270798: Failed to open file: " + filename );

  // Skip the header.
  in.seekg ( 80 );

  // Default locale.
  std::locale loc;

  // Keep count.
  unsigned long count ( 0 );

  // Loop through the file.
  while ( EOF != in.peek() )
  {
    // See if the character is not ascii.
    const char c ( in.get() );
    if ( false == std::isalnum<char> ( c, loc ) &&
         false == std::isdigit<char> ( c, loc ) &&
         false == std::isspace<char> ( c, loc ) &&
         '.' != c &&
         '-' != c &&
         '+' != c &&
         ':' != c &&
         '\\' != c &&
         'E' != c &&
         'e' != c )
    {
      // Binary character found.
      return false;
    }

    // Count the characters processed (for debugging).
    ++count;
  }

  // If we get this far then we did not find any binary characters.
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the binary file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_parseBinaryFile( std::ifstream &in )
{
  // Skip header.
  in.seekg ( 80 );

  const unsigned int size ( 512 );
  char buf[size];

  int numFacets = 0;
  
  in.read(buf, 4);

  ::memcpy(&numFacets, buf, 4);

  while ( numFacets > 0 )
  {
    float f[3];

    in.read(buf, 50);
    ::memcpy(f, buf, 12); // copy first 12 bytes for normal
    osg::Vec3 n ( f[0], f[1], f[2] );
    _polygons[3].second.push_back ( n );

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
  }
}
  

///////////////////////////////////////////////////////////////////////////////
//
//  Parse the ascii file.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterSTL::_parseAsciiFile( std::ifstream &in )
{
  const unsigned int size ( 512 );
  char buf[size];
  Vertices vertices;
  Normals normals;

  while( !in.eof() )
  {
    in.getline(buf, size - 1);

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
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterSTL::Result ReaderWriterSTL::_read ( const std::string &filename, const Options *options )
{
  // Make sure the internal data members are initialized.
  this->_init();

  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // See if the file is ascii.
  if ( this->_isAscii ( filename ) )
  {
    std::ifstream in ( filename.c_str() );
    if ( !in.is_open() )
      throw std::runtime_error ( "Error 2346450991: Failed to open ascii file: " + filename );
    this->_parseAsciiFile ( in );
  }

  // Otherwise, read the binary file.
  else
  {
    std::ifstream in ( filename.c_str(), std::ifstream::in | std::ifstream::binary );
    if ( !in.is_open() )
      throw std::runtime_error ( "Error 3162884175: Failed to open binary file: " + filename );
    this->_parseBinaryFile ( in );
  }

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write the file
//
///////////////////////////////////////////////////////////////////////////////
ReaderWriterSTL::WriteResult ReaderWriterSTL::writeNode(const osg::Node& node, const std::string& fileName, const Options* options) \
{ 
  std::string ext = osgDB::getFileExtension(fileName);
  if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;

  std::string chunk ( options->getOptionString() );

  if(chunk == "Binary")
    return _writeBinary( node, fileName );
  else if(chunk == "Ascii")
    return _writeAscii( node, fileName );
  return WriteResult::ERROR_IN_WRITING_FILE;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write an ascii file
//
///////////////////////////////////////////////////////////////////////////////
ReaderWriterSTL::WriteResult ReaderWriterSTL::_writeAscii  ( const osg::Node& node, const std::string& filename )
{
  std::ofstream fout ( filename.c_str() );
  if ( !fout.is_open() )
    return WriteResult::ERROR_IN_WRITING_FILE;

  fout << "solid " << filename << std::endl;
  //functor to write out facets
  AsciiWriter writer( fout );
  
  GeodeWriter<AsciiWriter> *geodeWriter = new GeodeWriter<AsciiWriter> ( writer );
  //visitor to write out file
  osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( OsgTools::MakeVisitor<osg::Geode>::make ( Usul::Adaptors::memberFunction ( geodeWriter, &GeodeWriter<AsciiWriter>::writeGeode ) ) );

  osg::Node &n = const_cast< osg::Node& > ( node );
  n.accept( *geodeVisitor);

  fout << "endsolid " << filename << std::endl;

  return WriteResult::FILE_SAVED;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write an binary file
//
///////////////////////////////////////////////////////////////////////////////
ReaderWriterSTL::WriteResult ReaderWriterSTL::_writeBinary ( const osg::Node& node, const std::string& filename )
{
  std::ofstream fout ( filename.c_str(), std::ofstream::out | std::ofstream::binary );
  if ( !fout.is_open() )
    return WriteResult::ERROR_IN_WRITING_FILE;

  char buf[84];//for the header and the number of facets

  ::memset( buf, 0, 84 );
  
  //create a header
  std::string header = "solid " + filename;
  ::memcpy( buf, header.c_str(), header.length() );

  FacetCounter *facetCounter = new FacetCounter();
  //visitor to count number of facets
  osg::ref_ptr<osg::NodeVisitor> facetVisitor ( OsgTools::MakeVisitor<osg::Geode>::make ( Usul::Adaptors::memberFunction ( facetCounter, &FacetCounter::countFacets ) ) );

  osg::Node &n = const_cast< osg::Node& > ( node );
  n.accept( *facetVisitor );

  //add number of facets to header
  unsigned int numFacets = facetCounter->getNumFacets();
  ::memcpy ( buf + 80, &numFacets, 4);

  fout.write( buf, 84 );

  //functor to write out facets
  BinaryWriter writer( fout );

  GeodeWriter<BinaryWriter> *geodeWriter = new GeodeWriter<BinaryWriter> ( writer );
  //visitor to write out file
  osg::ref_ptr<osg::NodeVisitor> geodeVisitor ( OsgTools::MakeVisitor<osg::Geode>::make ( Usul::Adaptors::memberFunction ( geodeWriter, &GeodeWriter<BinaryWriter>::writeGeode ) ) );
  n.accept( *geodeVisitor );

  return WriteResult::FILE_SAVED;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a geode's geometry to the file
//  TODO, only write out Highest LOD geometry.
//  TODO, convert other primitives to triangles.
//
///////////////////////////////////////////////////////////////////////////////
template < class Writer >
void ReaderWriterSTL::GeodeWriter< Writer >::writeGeode( osg::Geode *geode )
{
  //loop through the drawables
  unsigned int numDrawables ( geode->getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if(geometry)
    {
      const osg::Array *constarray = geometry->getVertexArray();
      osg::Array *array = const_cast < osg::Array*> ( constarray );
      osg::ref_ptr< osg::Vec3Array > vertices = dynamic_cast< osg::Vec3Array*> (array);
      const osg::Vec3Array *constvec3array = geometry->getNormalArray();
      osg::ref_ptr< osg::Vec3Array > normals  = const_cast<osg::Vec3Array *> (constvec3array);

      for(unsigned int j = 0; j < normals->size(); ++j)
      {
        _writer(normals->at(j), vertices->at(j*3), vertices->at(j*3+1), vertices->at(j*3+2));
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Add the number of facets in this geometry to the running total
//
///////////////////////////////////////////////////////////////////////////////
void ReaderWriterSTL::FacetCounter::countFacets( osg::Geode *geode )
{
  unsigned int numDrawables ( geode->getNumDrawables() );
  for ( unsigned int i = 0; i < numDrawables; ++i )
  {
    // Get the drawable.
    const osg::Drawable *drawable = geode->getDrawable ( i );

    // See if the drawable is a geometry.
    const osg::Geometry *geometry = drawable->asGeometry();
    if(geometry)
    {
      const osg::Vec3Array *normals = geometry->getNormalArray();

      _numFacets += normals->size();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in Ascii
//
///////////////////////////////////////////////////////////////////////////////
void ReaderWriterSTL::AsciiWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3)
{
  _out << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << " " << std::endl;
  _out << "  outer loop " << std::endl;
  _out << "    vertex "   << v1[0] << " " << v1[1] << " " << v1[2] << " " << std::endl;
  _out << "    vertex "   << v2[0] << " " << v2[1] << " " << v2[2] << " " << std::endl;
  _out << "    vertex "   << v3[0] << " " << v3[1] << " " << v3[2] << " " << std::endl;
  _out << "  endloop"     << std::endl;
  _out << "endfacet"      << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in binary
//
///////////////////////////////////////////////////////////////////////////////
void ReaderWriterSTL::BinaryWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3)
{
  char buf[50]; //50 bytes to define a facet
  ::memset( buf, 0, 50 );

  float f1, f2, f3;

  //write normal
  f1 = normal[0];
  f2 = normal[1];
  f3 = normal[2];
  ::memcpy( buf    , &f1  , 4);
  ::memcpy( buf + 4, &f2, 4);
  ::memcpy( buf + 8, &f3, 4);

  //write first vertex
  f1 = v1[0];
  f2 = v1[1];
  f3 = v1[2];
  ::memcpy( buf + 12, &f1  , 4);
  ::memcpy( buf + 16, &f2, 4);
  ::memcpy( buf + 20, &f3, 4);

  //write second vertex
  f1 = v2[0];
  f2 = v2[1];
  f3 = v2[2];
  ::memcpy( buf + 24, &f1  , 4);
  ::memcpy( buf + 28, &f2, 4);
  ::memcpy( buf + 32, &f3, 4);

  //write third vertex
  f1 = v3[0];
  f2 = v3[1];
  f3 = v3[2];
  ::memcpy( buf + 36, &f1  , 4);
  ::memcpy( buf + 40, &f2, 4);
  ::memcpy( buf + 44, &f3, 4);

  //write spacers
  ::memcpy( buf + 48, "\0", 1);
  ::memcpy( buf + 49, "\0", 1);

  _out.write( buf, 50 );
}

///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterSTL> g_ReaderWriter_STL_proxy;
