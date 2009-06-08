
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ReaderWriterYARN.h"

#include "osgDB/ReadFile"
#include "osgDB/Registry"
#include "osgDB/FileNameUtils"
#include "osgDB/FileUtils"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Material"

#include "OsgTools/MaterialFactory.h"

#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <algorithm>



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::ReaderWriterYARN() : BaseClass(),
  _yarns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::~ReaderWriterYARN()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does this plugin handle files with the given extension?
//
///////////////////////////////////////////////////////////////////////////////

bool ReaderWriterYARN::acceptsExtension ( const std::string &ext )
{
  return osgDB::equalCaseInsensitive ( ext, "yarn" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the class name.
//
///////////////////////////////////////////////////////////////////////////////

const char* ReaderWriterYARN::className()
{
  return "YARN Reader";
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the node.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::ReadResult ReaderWriterYARN::readNode ( const std::string &file, const Options *options )
{
  try
  {
    return this->_read ( file, options );
  }

  // Catch known exceptions.
  catch ( const ReaderWriterYARN::ReadResult &r )
  {
    return r;
  }

  // Catch standard exceptions.
  catch ( const std::exception &e )
  {
    return ReaderWriterYARN::ReadResult ( e.what() );
  }

  // Catch all other exceptions.
  catch ( ... )
  {
    return ReaderWriterYARN::ReadResult ( "Unknown exception caught" );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the triangle-fan.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *ReaderWriterYARN::_makeFan ( const Vec3 &center, const OsgTools::Mesh &mesh, unsigned int stack ) const
{
  const unsigned int size ( mesh.rows() + 1 );
  osg::ref_ptr<osg::Vec3Array> points ( new osg::Vec3Array ( size ) );
  osg::ref_ptr<osg::Vec3Array> normal ( new osg::Vec3Array ( 1 ) );

  points->at(0) = center;
  for ( unsigned int i = 1; i < size; ++i )
    points->at(i) = mesh.point ( i - 1, stack );

  Vec3 a ( points->at(1) - center );
  Vec3 b ( points->at(2) - center );
  Vec3 c ( a ^ b ); // Cross-product
  c.normalize();

  // Flip the normal if we are at the end.
  if ( 0 == stack )
    c *= -1;

  normal->at(0) = c;

  osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
  geometry->setVertexArray ( points.get() );
  geometry->setNormalArray ( normal.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_FAN, 0, points->size() ) );

  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );
  geode->addDrawable ( geometry.get() );
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Group *ReaderWriterYARN::_build() const
{
  // The scene root.
  osg::ref_ptr<osg::Group> root ( new osg::Group );

  // Material factory.
  OsgTools::MaterialFactory::Ptr materialFactory ( new OsgTools::MaterialFactory );

  // Loop through the meshes.
  for ( Yarns::const_iterator i = _yarns.begin(); i != _yarns.end(); ++i )
  {
    const CenterCurve &curve = i->first;
    const OsgTools::Mesh &mesh = i->second;

    // Generate the node.
    osg::ref_ptr<osg::Node> node ( mesh() );

    // Set the material.
    osg::ref_ptr<osg::StateSet> ss ( node->getOrCreateStateSet() );
    osg::ref_ptr<osg::Material> mat ( materialFactory->create() );
    ss->setAttribute ( mat.get() );

    // Add new mesh to the root.
    root->addChild ( node.get() );

    // Make a triangle-fan on the left side and set the material.
    osg::ref_ptr<osg::Node> cap ( this->_makeFan ( curve.front(), mesh, 0 ) );
    ss = cap->getOrCreateStateSet();
    ss->setAttribute ( mat.get() );
    root->addChild ( cap.get() );

    // Same on the right.
    cap = this->_makeFan ( curve.back(),  mesh, curve.size() - 1 );
    ss = cap->getOrCreateStateSet();
    ss->setAttribute ( mat.get() );
    root->addChild ( cap.get() );
  }

  // Return the root.
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize internal data structures.
//
///////////////////////////////////////////////////////////////////////////////

void  ReaderWriterYARN::_init()
{
  _yarns.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read file.
//
///////////////////////////////////////////////////////////////////////////////

ReaderWriterYARN::ReadResult ReaderWriterYARN::_read ( const std::string &filename, const Options *options )
{
  // Make sure we handle files with this extension.
  if ( !this->acceptsExtension ( osgDB::getFileExtension ( filename ) ) )
    return ReadResult::FILE_NOT_HANDLED;

  // Make sure the internal data members are initialized.
  this->_init();

  // Open the file.
  std::ifstream in ( filename.c_str() );
  if ( !in.is_open() )
    throw std::runtime_error ( "Error 2346450991: Failed to open file: " + filename );

  // Parse the file.
  this->_parseYarns ( in );

  // Build the scene.
  osg::ref_ptr<osg::Group> root ( this->_build() );

  // Initialized again to free accumulated data.
  this->_init();

  // Return the scene
  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the yarns.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseYarns ( std::ifstream& fin )
{
  const unsigned int size ( 1024 );
  char buf[size];

  unsigned int numYarns ( 0 );
  unsigned int numSlices ( 0 );

  while ( !fin.eof() )
  {
    // Read the line.
    fin.getline ( buf, size );

    // Get the first word.
    std::istringstream in ( buf );
    std::string word;
    in >> word;

    // Convert to lower-case.
    std::transform ( word.begin(), word.end(), word.begin(), ::tolower );

    if ( word == "total_yarn_number" )
    {
      in >> numYarns;
      _yarns.resize ( numYarns );
    }

    else if ( word == "number_of_slices" )
    {
      in >> numSlices;
    }

    else if ( word == "yarn_number" )
    {
      this->_parseNumStacks ( fin, numYarns, numSlices );
    }

    else if ( word == "center_point" )
    {
      osg::Vec3 center;
      in >> center[0] >> center[1] >> center[2];
      this->_parseStack ( fin, numSlices, center );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the number of stacks.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseNumStacks ( std::ifstream& fin, unsigned int numYarns, unsigned int numSlices )
{
  unsigned int yarn, stacks;
  ++numSlices; // We close the loop.

  for ( unsigned int i = 0; i < numYarns; ++i )
  {
    fin >> yarn >> stacks;
    _yarns.at(yarn).second.points().resize  ( stacks * numSlices );
    _yarns.at(yarn).second.normals().resize ( stacks * numSlices );
    _yarns.at(yarn).second.size ( numSlices, stacks );
    _yarns.at(yarn).first.resize ( stacks ); // Number of points in center-curve.
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Parse the stack.
//
///////////////////////////////////////////////////////////////////////////////

void ReaderWriterYARN::_parseStack ( std::ifstream& fin, unsigned int numSlices, const osg::Vec3 &center )
{
  unsigned int yarn, stack, slice;
  float x, y, z;

  for ( unsigned int i = 0; i < numSlices; ++i )
  {
    // Read the data.
    fin >> yarn >> stack >> slice >> x >> y >> z;

    // Set the vertex.
    Vertex &v = _yarns.at(yarn).second.point ( slice, stack );
    v[0] = x;
    v[1] = y;
    v[2] = z;

    // Set the normal.
    Normal &n = _yarns.at(yarn).second.normal ( slice, stack );
    n = v - center;
    n.normalize();
  }

  // Close the loop by setting the last vertex to be the first.
  _yarns.at(yarn).second.point  ( numSlices, stack ) = _yarns.at(yarn).second.point  ( 0, stack );
  _yarns.at(yarn).second.normal ( numSlices, stack ) = _yarns.at(yarn).second.normal ( 0, stack );

  // Set the center point.
  _yarns.at(yarn).first.at(stack) = center;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Register this class as an OSG plugin.
//
///////////////////////////////////////////////////////////////////////////////

osgDB::RegisterReaderWriterProxy<ReaderWriterYARN> g_ReaderWriter_YARN_proxy;
