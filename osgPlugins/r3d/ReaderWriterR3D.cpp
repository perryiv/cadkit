
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

#include "Usul/IO/Reader.h"
#include "Usul/Math/Vector3.h"

#include <fstream>
#include <stdexcept>
#include <sstream>


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::ReaderWriterR3D() : BaseClass(),
  _vertices(),
  _normals()
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

osg::Group *ReaderWriterR3D::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // A single geode.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  root->addChild ( geode.get() );

  // Shortcuts to the vertices and normals.
  const Vectors &v = _vertices;
  const Vectors &n = _normals;

  // Check the numbers.
  if ( n.size() * 3 != v.size() )
  {
    std::ostringstream out;
    out << "Error 2993317852: number of normals times 3 = " << n.size() << " but number of vertices is " << v.size();
    throw std::runtime_error ( out.str() );
  }

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
  _vertices.clear();
  _normals.clear();
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

  // Initialize again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}

//Crashes if you keep-all-connected and pick "yes" when it asks to save when exiting.


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
//  Note: even though the VTK file holds tri-strip information, there are so 
//  many of them that, at least with OSG, it will probably be faster to draw 
//  individual triangles. This is because glDrawArrays() is used internally. 
//  Don't think OSG supports packing the tri-strips end-to-end in a single 
//  osg::Geometry and passing this to OpenGL in an efficient way.
//
//  Note: Existing algorithms for delete-all-connected assume a single 
//  osg::Geometry of triangles, which is another reason to build the scene 
//  in this way.
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
  unsigned int numPoints ( 0 );
  in >> numPoints;

  // Should be floating point data.
  this->_check ( in, "float" );
  this->_skipLine ( in );

  // Make a vertex pool.
  Vectors pool ( numPoints );

  // Needed in the loop.
  Usul::Types::Float32 x, y, z;

  // Start reading the vertices.
  for ( unsigned int i = 0; i < numPoints; ++i )
  {
    Usul::IO::ReadBigEndian::read ( in, x, y, z );
    pool.at(i).set ( x, y, z );
  }

  // Make sure it is a collection of tri-strips.
  this->_check ( in, "triangle_strips" );
  Usul::Types::Uint32 numStrips, dummy;
  in >> numStrips >> dummy;
  this->_skipLine ( in );

  // Needed in the loop.
  typedef std::vector < unsigned int > Indices;
  typedef std::vector < Indices > Strips;
  Strips strips ( numStrips );
  unsigned int numTriangles ( 0 );

  // Start reading the strips.
  for ( unsigned int j = 0; j < numStrips; ++j )
  {
    // Read how many indices are in this strip.
    Usul::Types::Uint32 numIndices ( 0 );
    Usul::IO::ReadBigEndian::read ( in, numIndices );
    strips.at(j).resize ( numIndices );
    numTriangles += ( numIndices - 2 );

    // Loop through all the indices.
    for ( unsigned int k = 0; k < numIndices; ++k )
    {
      // Read the index.
      Usul::Types::Uint32 index ( 0 );
      Usul::IO::ReadBigEndian::read ( in, index );
      strips.at(j).at(k) = index;
    }
  }

  // Size the vertices.
  const unsigned int numVertices ( numTriangles * 3 );
  _vertices.resize ( numVertices );
  _normals.resize ( numTriangles );

  // Set all the vertices and normals.
  unsigned int vc ( 0 ), tc ( 0 );
  for ( unsigned int s = 0; s < numStrips; ++s )
  {
    // Loop through each strip.
    const Indices &indices = strips.at(s);
    const unsigned int numIndices ( indices.size() );
    const unsigned int stop ( numIndices - 2 );
    for ( unsigned int k = 0; k < stop; ++k )
    {
      // Get the three indices.
      const unsigned int &i0 = indices.at(k);
      const unsigned int &i1 = indices.at(k+1);
      const unsigned int &i2 = indices.at(k+2);

      // Get the three vertices.
      const osg::Vec3 &v0 = pool.at(i0);
      const osg::Vec3 &v1 = pool.at(i1);
      const osg::Vec3 &v2 = pool.at(i2);

      // Set the vertices in the big list.
      _vertices.at(vc++).set ( v0 );
      _vertices.at(vc++).set ( v1 );
      _vertices.at(vc++).set ( v2 );

      // Calculate the normal. Every other one we flip.
      const osg::Vec3 v01 ( v1 - v0 );
      const osg::Vec3 v02 ( v2 - v0 );
      osg::Vec3 n ( ( k % 2 ) ? v02 ^ v01 : v01 ^ v02 );
      n.normalize();
      _normals.at(tc++).set ( n );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterR3D> g_ReaderWriter_R3D_proxy;
