
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

#include "osgUtil/Optimizer"

#include "OsgTools/Visitor.h"
#include "OsgTools/SetDataVariance.h"

#include "Usul/IO/Reader.h"
#include "Usul/Math/Vector3.h"
#include "Usul/File/Path.h"
#include "Usul/File/Temp.h"

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
//  Write the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::WriteResult ReaderWriterR3D::writeNode ( const osg::Node &node, const std::string &filename, const Options *options )
{
  try
  {
    return this->_write ( node, filename, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterR3D::WriteResult &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterR3D::WriteResult ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterR3D::WriteResult ( "Unknown exception caught" );
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

  //No update functor
  Usul::Adaptors::ProgressBarUpdate progress ( 0x0 );

  // Parse the file.
  this->_parse ( filename, progress );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialize again to free accumulated data.
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
//  Note: even though the VTK file holds tri-strip information, there are so 
//  many of them that, at least with OSG, it will probably be faster to draw 
//  individual triangles, which will use glDrawArrays() internally. 
//  Don't think OSG supports packing the tri-strips end-to-end in a single 
//  osg::Geometry and passing this to OpenGL in an efficient way.
//
//  Note: Existing algorithms for delete-all-connected assume a single 
//  osg::Geometry of triangles, which is another reason to build the scene 
//  in this way.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterR3D::_parse (  const std::string& filename, const Usul::Adaptors::ProgressBarUpdate& progress  )
{
  // Open the binary file.
  std::ifstream in ( filename.c_str(), std::ifstream::in | std::ifstream::binary );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2260991012: Failed to open binary file: " + filename );

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

  // Time of last updating.
  double lastTime ( ::clock() );

  // Start reading the vertices.
  for ( unsigned int i = 0; i < numPoints; ++i )
  {
    Usul::IO::ReadBigEndian::read ( in, x, y, z );
    pool.at(i).set ( x, y, z );

    if ( ( ::clock() - lastTime ) > 500 )
    {
      // Update the progress.
      progress ( ( (float) i / numPoints ) * 50 );
      lastTime = ::clock();
    }
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

    if ( ( ::clock() - lastTime ) > 500 )
    {
      // Update the progress.
      progress ( 50 + ( (float) j / numStrips * 50 ) );
      lastTime = ::clock();
    }
  }

  typedef std::vector < osg::Vec3f > Vectors;

  Vectors vertices;
  Vectors normals;

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
//  Write the R3D file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterR3D::WriteResult ReaderWriterR3D::_write ( const osg::Node &node, const std::string &name, const Options *options )
{
  // Check extension.
  std::string ext = osgDB::getFileExtension ( name );
  if ( !this->acceptsExtension ( ext ) )
    return WriteResult::FILE_NOT_HANDLED;

#ifdef _DEBUG

  // Make a temporary file name.
  Usul::File::Temp file ( Usul::File::Temp::BINARY );

  // Build collection of unique vertices and triangles.
  //Usul::Algorithms::Polygons::

  // Make a copy of the scene.
  osg::ref_ptr<osg::Node> copy ( dynamic_cast < osg::Node * > ( node.clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Traverse the scene and change all transforms to static data-variance.
  typedef OsgTools::SetDataVariance SetDataVariance;
  typedef OsgTools::Visitor < osg::Transform, SetDataVariance > VarianceVisitor;
  SetDataVariance setter ( osg::Object::STATIC );
  VarianceVisitor::Ptr vv ( new VarianceVisitor ( setter ) );
  copy->accept ( *vv );

  // Flatten static transforms and make tri-strips.
  osgUtil::Optimizer optimizer;
  optimizer.optimize ( copy.get(), osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS | osgUtil::Optimizer::TRISTRIP_GEOMETRY );

  // Write the header.
  file.stream() << "# vtk DataFile Version 3.0\n";
  file.stream() << "vtk output\n";
  file.stream() << "BINARY\n";
  file.stream() << "DATASET POLYDATA\n";

#if 0

  // Our collection of vertices and indices.
  typedef std::vector<osg::Vec3f> Vertices;
  typedef std::vector<unsigned int> Indices;

  // Traverse the scene and collect the vertices and indices.
  typedef 
  typedef OsgTools::ForEach < WriteR3D, osg::Drawable > Operation;
  typedef OsgTools::Visitor < osg::Geode, Operation > Visitor;

  // Make the visitor.
  WriteR3D write ( out );
  Operation op ( write );
  Visitor::Ptr visitor ( new Visitor ( op ) );

#endif

  // If we get this far then rename the temporary file.
  file.rename ( name );

  // Success.
  return WriteResult::FILE_SAVED;

#else // Release

  throw std::runtime_error ( "Error 3001353581: Writing R3D files not implemented" );
  return WriteResult::FILE_NOT_HANDLED;

#endif
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterR3D> g_ReaderWriter_R3D_proxy;
