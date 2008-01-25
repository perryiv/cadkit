
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
#include "osg/Vec2"
#include "osg/Vec3"
#include "osg/Vec4"

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
  typedef osg::Vec2 TexCoord;
  typedef std::vector < TexCoord > TexCoords;
  typedef osg::Vec4 Color;
  typedef std::vector < Color > Colors;

  Mesh();
  Mesh ( unsigned int rows, unsigned int columns );
  Mesh ( const Mesh & );
  Mesh &operator = ( const Mesh & );

  // Allocate the mesh.
  void                allocatePoints();
  void                allocateNormals();
  void                allocateTexCoords();
  void                allocateColors();

  // Access to the colors.
  void                colors ( const Colors &p ) { _colors = p; }
  Colors &            colors()       { return _colors; }
  const Colors &      colors() const { return _colors; }

  // Access to a single color.
  Color&              color ( size_type row, size_type column );
  const Color&        color ( size_type row, size_type column ) const;

  // The number of columns.
  unsigned int        columns() const { return _columns; }

  // Normalize all the normal vectors (make them have a length of one).
  void                normalize();

  // Access to the normals.
  void                normals ( const Vectors &n ) { _normals = n; }
  Vectors &           normals()       { return _normals; }
  const Vectors &     normals() const { return _normals; }

  // Access to a single normal.
  reference           normal ( size_type row, size_type column );
  const_reference     normal ( size_type row, size_type column ) const;

  // Access to the points.
  void                points ( const Vectors &p ) { _points = p; }
  Vectors &           points()       { return _points; }
  const Vectors &     points() const { return _points; }

  // Access to a single point.
  reference           point ( size_type row, size_type column );
  const_reference     point ( size_type row, size_type column ) const;

  // The number of rows.
  unsigned int        rows() const { return _rows; }

  // Set the size.
  void                size ( unsigned int rows, unsigned int columns );

  // Access to the texture coordinates.
  void                texCoords ( const TexCoords &p ) { _texCoords = p; }
  TexCoords &         texCoords()       { return _texCoords; }
  const TexCoords &   texCoords() const { return _texCoords; }

  // Access to a single texture coordinate.
  TexCoord&           texCoord ( size_type row, size_type column );
  const TexCoord&     texCoord ( size_type row, size_type column ) const;

  // Generate a scene.
  osg::Node *         operator()() const;

  // Get/Set the use color flag.
  void                useColors ( bool b );
  bool                useColors () const;
private:

  Vectors _points;
  Vectors _normals;
  TexCoords _texCoords;
  Colors _colors;
  unsigned int _rows;
  unsigned int _columns;
  bool _useColors;
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
  _columns   ( columns ),
  _useColors ( false )
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
  //  Allocate the points.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template<class VertexType>
  void Mesh<VertexType>::allocatePoints()
  {
    _points.resize ( _columns * _rows );
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Allocate the normals.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template<class VertexType>
  void Mesh<VertexType>::allocateNormals()
  {
    _normals.resize ( _columns * _rows );
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Allocate the texture coordinates.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template<class VertexType>
  void Mesh<VertexType>::allocateTexCoords()
  {
    _texCoords.resize ( _columns * _rows );
  }
  
  
  ///////////////////////////////////////////////////////////////////////////////
  //
  //  Normalize all the normals.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
  template<class VertexType>
  void Mesh<VertexType>::normalize()
  {
    for ( typename Vectors::iterator i = _normals.begin(); i != _normals.end(); ++i )
      i->normalize();
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
  //  Figure out the normal binding.
  //
  ///////////////////////////////////////////////////////////////////////////////
  
    namespace Detail
    {
      template < class Sequence > 
      osg::Geometry::AttributeBinding normalBinding ( const Sequence &points, const Sequence &normals )
      {
        // Initialize.
        osg::Geometry::AttributeBinding binding ( osg::Geometry::BIND_OFF );
        
        // Check the respective numbers.
        if ( points.size() == normals.size() )
          binding = osg::Geometry::BIND_PER_VERTEX;
        else if ( 1 == normals.size() )
          binding = osg::Geometry::BIND_OVERALL;
        else if ( 0 == normals.size() )
          binding = osg::Geometry::BIND_OFF;
        else
          ErrorChecker ( 1826625791u, false, "Unsupported normal binding" );
        
        // Return the binding flag.
        return binding;
      }
    };
  
  
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
    
    // Figure out the normal binding.
    typedef osg::Geometry::AttributeBinding Binding;
    Binding binding ( Detail::normalBinding ( _points, _normals ) );
    
    // Declare nodes.
    osg::ref_ptr<osg::Geode>    geode ( new osg::Geode );
    osg::ref_ptr<osg::Geometry> geom  ( new osg::Geometry );
    
    // Allocate the points.
    unsigned int numVertices ( ( 2 * _rows - 2 ) * _columns );
    osg::ref_ptr<osg::Vec3Array> points  ( new osg::Vec3Array ( numVertices ) );
    
    // Figure out how many normals we need.
    unsigned int numNormals ( 0 );
    if ( osg::Geometry::BIND_PER_VERTEX == binding )
      numNormals = numVertices;
    else if ( osg::Geometry::BIND_OVERALL == binding )
      numNormals = 1;
    
    // Allocate the normals.
    osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( numNormals  ) );
    
    // If the binding is overall, take care of it now.
    if ( osg::Geometry::BIND_OVERALL == binding )
      normals->at ( 0 ) = this->normal ( 0, 0 );
    
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
        
        // Set the normal if we should.
        if ( osg::Geometry::BIND_PER_VERTEX == binding )
        {
          normals->at ( index - 2 ) = this->normal ( i + 1, j );
          normals->at ( index - 1 ) = this->normal ( i,     j );
        }
      }
      
      // Define the primitive.
      primSetList[i] = new osg::DrawArrays ( osg::PrimitiveSet::TRIANGLE_STRIP, start, length );
      start += length;
    }
    
    // Should be true.
    USUL_ASSERT ( points->size() == index );
    USUL_ASSERT ( index == start );
    
    // Set the points.
    geom->setVertexArray ( points.get() );
    
    // Set the normals.
    geom->setNormalArray ( normals.get() );
    geom->setNormalBinding ( binding );
    
    // Set the texture coordinates.
    geom->setTexCoordArray ( 0, texCoords.get() );
    
    // Set the primitive-set list.
    geom->setPrimitiveSetList ( primSetList );
    
    // Tell it to use color instead of materials.
#if 0
    osg::ref_ptr<osg::StateSet> ss ( geode->getOrCreateStateSet() );
    ss->setMode ( GL_LIGHTING, osg::StateAttribute::OFF );
    osg::ref_ptr<osg::Vec4Array> colors ( new osg::Vec4Array ( 1 ) );
    colors->at ( 0 ).set ( 1, 0, 0, 1 );
    geom->setColorArray ( colors.get() );
    geom->setColorBinding ( osg::Geometry::BIND_OVERALL );
#endif
    
    // Add the drawable.
    geode->addDrawable ( geom.get() );
    
    // TODO. For now... without this rendering polygons as "lines" draws a couple of weird extra line. OSG bug?
    geom->setUseDisplayList ( false );
    
    // Use vertex buffers.
    //geom->setUseVertexBufferObjects ( true );
    
    // Release the geode and return it.
    return geode.release();
  }
  
}; // namespace OsgTools


#endif // _STAR_SYSTEM_SURFACE_MESH_BUILDER_H_
