
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to make a surface mesh.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Mesh.h"
#include "OsgTools/ErrorChecker.h"

#include "osg/Geode"
#include "osg/Geometry"

#include "Usul/Exceptions/Thrower.h"

#include <fstream>
#include <iomanip>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::Mesh() :
  _points(),
  _normals(),
  _rows(),
  _columns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::Mesh ( const Mesh &m ) :
  _points  ( m._points  ),
  _normals ( m._normals ),
  _rows    ( m._rows    ),
  _columns ( m._columns )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

Mesh &Mesh::operator = ( const Mesh &m )
{
  _points  = m._points;
  _normals = m._normals;
  _rows    = m._rows;
  _columns = m._columns;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::size ( unsigned int rows, unsigned int columns )
{
  _columns = columns;
  _rows    = rows;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Normalize all the normals.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::normalize()
{
  for ( Vectors::iterator i = _normals.begin(); i != _normals.end(); ++i )
    i->normalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::reference Mesh::point ( size_type r, size_type c )
{
  return _points.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::const_reference Mesh::point ( size_type r, size_type c ) const
{
  return _points.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single normal.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::reference Mesh::normal ( size_type r, size_type c )
{
  return _normals.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single normal.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::const_reference Mesh::normal ( size_type r, size_type c ) const
{
  return _normals.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Reads a simple ascii text file.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::read ( const std::string &filename )
{
  // Open the file.
  std::ifstream in ( filename.c_str(), std::ios::in );
  if ( !in )
  {
    Usul::Exceptions::Thrower<std::runtime_error>
      ( "Error 1389143263, failed to open.",
        "\n\tFile: ", filename );
  }

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

  // Read the points.
  Vectors points;
  points.resize ( rows * columns );
  for ( Vectors::size_type i = 0; i < points.size(); ++i )
  {
    // Make sure there are more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3418646572, there should be ", points.size(), " data points.",
          "\n\tFile: ", filename );
    }

    // Set this element.
    in >> points[i][0] >> points[i][1] >> points[i][2];
  }

  // Read the number of normals.
  unsigned int numNormals ( 0 );
  in >> numNormals;

  // Read the normals.
  Vectors normals;
  normals.resize ( numNormals );
  for ( Vectors::size_type j = 0; j < normals.size(); ++j )
  {
    // Make sure there are more.
    if ( in.fail() || in.eof() )
    {
      Usul::Exceptions::Thrower<std::runtime_error>
        ( "Error 3070575716, there should be ", normals.size(), " normals.",
          "\n\tFile: ", filename );
    }

    // Set this element.
    in >> normals[j][0] >> normals[j][1] >> normals[j][2];
  }

  // Assign the members.
  _rows    = rows;
  _columns = columns;
  _points  = points;
  _normals = normals;

  // Normalizs the normals.
  this->normalize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Figure out the normal binding.
//
///////////////////////////////////////////////////////////////////////////////

namespace OsgTools
{
  namespace Detail
  {
    template < class Sequence > 
    osg::Geometry::AttributeBinding normalBinding ( unsigned int numVertsPerPrim, const Sequence &points, const Sequence &normals )
    {
      // Initialize.
      osg::Geometry::AttributeBinding binding ( osg::Geometry::BIND_OFF );

      // Check the respective numbers.
      if ( points.size() == normals.size() )
        binding = osg::Geometry::BIND_PER_VERTEX;
      else if ( points.size() == normals.size() / numVertsPerPrim )
        binding = osg::Geometry::BIND_PER_PRIMITIVE;
      else if ( 1 == normals.size() )
        binding = osg::Geometry::BIND_OVERALL;
      else
        ErrorChecker ( 1826625791u, false, "Unable to determine normal binding" );

      // Return the binding flag.
      return binding;
    }
  };
};


///////////////////////////////////////////////////////////////////////////////
//
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Mesh::operator()() const
{
  // Check state.
  ErrorChecker ( 4066213134u, _rows * _columns == _points.size() );

  // Figure out the normal binding.
  unsigned int numVertsPerPrim ( 3 );
  typedef osg::Geometry::AttributeBinding Binding;
  Binding binding ( Detail::normalBinding ( numVertsPerPrim, _points, _normals ) );

  // Declare nodes.
  osg::ref_ptr<osg::Geode>     geode   ( new osg::Geode );
  osg::ref_ptr<osg::Geometry>  geom    ( new osg::Geometry );

  // Allocate the points.
  unsigned int numVertices ( ( 2 * _rows - 2 ) * _columns );
  osg::ref_ptr<osg::Vec3Array> points ( new osg::Vec3Array ( numVertices ) );

  // There is one tri-strip for each adjacent pair of rows.
  osg::Geometry::PrimitiveSetList primSetList ( _rows - 1 );

  // Used in the loop.
  unsigned int index ( 0 );
  unsigned int length ( 2 * _columns ), start ( 0 );

  // Loop through all the rows.
  for ( unsigned int i = 0; i < _rows - 1; ++i )
  {
    // Loop through all the columns.
    for ( unsigned int j = 0; j < _columns; ++j )
    {
      // Make sure we are in range.
      USUL_ASSERT ( index < points->size() );

      // Set the "upper" and "lower" points. (Think of a regular grid on paper,
      // marching left-to-right, from the bottom towards the top.)
      points->at ( index++ ) = this->point ( i,     j );
      points->at ( index++ ) = this->point ( i + 1, j );
    }

    primSetList[i] = new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, start, length );
    start += length;
  }

  // Should be true.
  USUL_ASSERT ( points->size() == index );
  USUL_ASSERT ( index == start );

  // Set the points.
  geom->setVertexArray ( points.get() );

  // Set the normals.
  //geom->setNormalArray ( normals.get() );
  //geom->setNormalBinding ( binding );

  // Set the primitive-set list.
  geom->setPrimitiveSetList ( primSetList );

  // Tell it to use color instead of materials.
  osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
  ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
  osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( 1 ) );
  colors->at ( 0 ).set ( 1, 0, 0, 1 );
  geom->setColorArray ( colors.get() );
  geom->setColorBinding ( osg::Geometry::BIND_OVERALL );

  // Add the drawable.
  geode->addDrawable ( geom.get() );

  // Release the geode and return it.
  return geode.release();
}
