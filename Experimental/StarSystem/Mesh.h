
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

#ifndef _STAR_SYSTEM_SURFACE_MESH_BUILDER_H_
#define _STAR_SYSTEM_SURFACE_MESH_BUILDER_H_

#include "Usul/Errors/Checker.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec2d"

#include <vector>

namespace osg { class Node; }

namespace StarSystem {


template <class VertexType>
class Mesh
{
public:

  // Useful typedefs.
  typedef VertexType Vector;
  typedef std::vector<Vector> Vectors;
  typedef typename Vectors::reference reference;
  typedef typename Vectors::const_reference const_reference;
  typedef typename Vectors::value_type value_type;
  typedef typename Vectors::size_type size_type;
  typedef osg::Vec2d TexCoord;
  typedef std::vector < TexCoord > TexCoords;

  Mesh ( unsigned int rows, unsigned int columns );

  // The number of columns.
  unsigned int        columns() const { return _columns; }

  // Access to a single normal.
  reference           normal ( size_type row, size_type column );
  const_reference     normal ( size_type row, size_type column ) const;

  // Access to a single point.
  reference           point ( size_type row, size_type column );
  const_reference     point ( size_type row, size_type column ) const;

  // The number of rows.
  unsigned int        rows() const { return _rows; }

  // Set the size.
  void                size ( unsigned int rows, unsigned int columns );

  // Access to a single texture coordinate.
  TexCoord&           texCoord ( size_type row, size_type column );
  const TexCoord&     texCoord ( size_type row, size_type column ) const;

  // Generate a scene.
  osg::Node *         operator()() const;

private:
  
  Mesh();
  Mesh ( const Mesh & );
  Mesh &operator = ( const Mesh & );
  
  Vectors _points;
  Vectors _normals;
  TexCoords _texCoords;
  unsigned int _rows;
  unsigned int _columns;
};

  
typedef Usul::Errors::Checker ErrorChecker;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
Mesh<VertexType>::Mesh() :
_points(),
_normals(),
_texCoords(),
_rows(),
_columns()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
Mesh<VertexType>::Mesh ( unsigned int rows, unsigned int columns ) :
_points    ( rows * columns ),
_normals   ( rows * columns ),
_texCoords ( rows * columns ),
_rows      ( rows ),
_columns   ( columns )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy constructor.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
Mesh<VertexType>::Mesh ( const Mesh &m ) :
_points    ( m._points    ),
_normals   ( m._normals   ),
_texCoords ( m._texCoords ),
_rows      ( m._rows      ),
_columns   ( m._columns   )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Assignment.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
Mesh<VertexType> &Mesh<VertexType>::operator = ( const Mesh &m )
{
  _points    = m._points;
  _normals   = m._normals;
  _texCoords = m._texCoords;
  _rows      = m._rows;
  _columns   = m._columns;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the size.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
void Mesh<VertexType>::size ( unsigned int rows, unsigned int columns )
{
  _columns = columns;
  _rows    = rows;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType> 
typename Mesh<VertexType>::reference Mesh<VertexType>::point ( size_type r, size_type c )
{
  return _points.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
typename Mesh<VertexType>::const_reference Mesh<VertexType>::point ( size_type r, size_type c ) const
{
  return _points.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single normal.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
typename Mesh<VertexType>::reference Mesh<VertexType>::normal ( size_type r, size_type c )
{
  return _normals.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single normal.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
typename Mesh<VertexType>::const_reference Mesh<VertexType>::normal ( size_type r, size_type c ) const
{
  return _normals.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single texture coordinate.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
typename Mesh<VertexType>::TexCoord& Mesh<VertexType>::texCoord ( size_type r, size_type c )
{
  return _texCoords.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single texture coordinate.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
const typename Mesh<VertexType>::TexCoord& Mesh<VertexType>::texCoord ( size_type r, size_type c ) const
{
  return _texCoords.at ( r * _columns + c );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

template<class VertexType>
osg::Node* Mesh<VertexType>::operator()() const
{
  // Check state.
  ErrorChecker ( 4066213134u, _rows * _columns == _points.size() );

  // Declare nodes.
  osg::ref_ptr<osg::Geode>    geode ( new osg::Geode );
  osg::ref_ptr<osg::Geometry> geom  ( new osg::Geometry );
  
#if 0
  // Allocate the points.
  const unsigned int numVertices ( ( 2 * _rows - 2 ) * _columns );
  osg::ref_ptr<osg::Vec3Array> points  ( new osg::Vec3Array ( numVertices ) );
  
  // Allocate the normals.
  const unsigned int numNormals ( numVertices );
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( numNormals  ) );
  
  // Allocation texture coordinates.
  osg::ref_ptr<osg::Vec2Array> texCoords ( new osg::Vec2Array ( numVertices ) );
  
  // There is one tri-strip for each adjacent pair of rows.
  osg::Geometry::PrimitiveSetList primSetList ( _rows - 1 );
  
  // Used in the loop.
  unsigned int index ( 0 );
  unsigned int length ( 2 * _columns ), start ( 0 );
  
  // Loop through all the rows.
  for ( unsigned int i = 0; i < primSetList.size(); ++i )
  {
    // Loop through all the columns.
    for ( unsigned int j = 0; j < _columns; ++j )
    {
      // Make sure we are in range.
      USUL_ASSERT ( index < points->size() );
      
      // Set the "upper" and "lower" points. (Think of a regular grid on paper,
      // marching left-to-right, from the bottom towards the top.)
      points->at ( index++ ) = this->point ( i + 1, j );
      points->at ( index++ ) = this->point ( i,     j );
      
      // Set the texture coordinates.
      texCoords->at ( index - 2 ) = this->texCoord ( i + 1, j );
      texCoords->at ( index - 1 ) = this->texCoord ( i,     j );
      
      // Set the normal.
      normals->at ( index - 2 ) = this->normal ( i + 1, j );
      normals->at ( index - 1 ) = this->normal ( i,     j );
    }
    
    // Define the primitive.
    primSetList[i] = new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, start, length );
    start += length;
  }
  
  // Should be true.
  USUL_ASSERT ( points->size() == index );
  USUL_ASSERT ( index == start );
#else

  // Allocate the points.
  osg::ref_ptr<osg::Vec3Array> points  ( new osg::Vec3Array (  _points.begin(), _points.end() ) );
  
  // Allocate the normals.
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( _normals.begin(), _normals.end()  ) );
  
  // Allocation texture coordinates.
  osg::ref_ptr<osg::Vec2Array> texCoords ( new osg::Vec2Array ( _texCoords.begin(), _texCoords.end() ) );
  
  // There is one tri-strip for each adjacent pair of rows.
  osg::Geometry::PrimitiveSetList primSetList ( _rows - 1 );
  
  // Loop through all the rows.
  for ( unsigned int i = 0; i < primSetList.size(); ++i )
  {
    osg::ref_ptr<osg::DrawElementsUShort> drawElements ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP ) );
    
    // Loop through all the columns.
    for ( unsigned int j = 0; j < _columns; ++j )
    {
      drawElements->push_back ( ( ( i + 1 ) * _columns ) + j );
      drawElements->push_back ( ( ( i     ) * _columns ) + j );
    }
    
    // Define the primitive.
    primSetList[i] = drawElements.get();
  }
#endif
  
  // Set the points.
  geom->setVertexArray ( points.get() );
  
  // Set the normals.
  geom->setNormalArray ( normals.get() );
  geom->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
  
  // Set the texture coordinates.
  geom->setTexCoordArray ( 0, texCoords.get() );
  
  // Set the primitive-set list.
  geom->setPrimitiveSetList ( primSetList );

  // Add the drawable.
  geode->addDrawable ( geom.get() );
  
  // TODO. For now... without this rendering polygons as "lines" draws a couple of weird extra line. OSG bug?
  geom->setUseDisplayList ( false );
  
  // Use vertex buffers.
  //geom->setUseVertexBufferObjects ( true );
  
  // Release the geode and return it.
  return geode.release();
}

}; // namespace StarSystem


#endif // _STAR_SYSTEM_SURFACE_MESH_BUILDER_H_
