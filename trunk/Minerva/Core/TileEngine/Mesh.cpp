
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

#include "Minerva/Core/TileEngine/Mesh.h"
#include "Minerva/Core/TileEngine/Body.h"

#include "osg/Geode"
#include "osg/Geometry"

using namespace Minerva::Core::TileEngine;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::Mesh ( unsigned int rows, unsigned int columns, double skirtHeight ) :
  _points    ( rows * columns * 2 ),
  _normals   ( rows * columns * 2 ),
  _texCoords ( rows * columns * 2 ),
  _rows      ( rows ),
  _columns   ( columns ),
  _skirtHeight ( skirtHeight )
{
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
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::operator() ( osg::Geode& mesh, osg::Geode& skirts ) const
{
  // Allocate the points.
  osg::ref_ptr<osg::Vec3Array> points  ( new osg::Vec3Array (  _points.begin(), _points.end() ) );
  
  // Allocate the normals.
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( _normals.begin(), _normals.end()  ) );
  
  // Allocation texture coordinates.
  osg::ref_ptr<osg::Vec2Array> texCoords ( new osg::Vec2Array ( _texCoords.begin(), _texCoords.end() ) );

  // Make the main mesh.
  {
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

    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
    
    // Set the points.
    geometry->setVertexArray ( points.get() );
    
    // Set the normals.
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    
    // Set the texture coordinates.
    geometry->setTexCoordArray ( 0, texCoords.get() );
    
    // Set the primitive-set list.
    geometry->setPrimitiveSetList ( primSetList );
        
    // Use vertex buffers.
    geometry->setUseDisplayList ( false );
    geometry->setUseVertexBufferObjects ( true );

    // Add the drawable.
    mesh.addDrawable ( geometry.get() );
  }

  // Make the skirts.\
  // Consider each square detail level of
	// the tile as a square with corners
	// A,B,C & D as in the figure below

  /*
		  D -------- C
	  ^	|          |
	  |	|          |
	  p	|          |
	  o	|          |
	  s	|          |
		  A -------- B
	  y
		  pos x ->
  */
  {
    // Get the number of vertices.
    const Vectors::size_type numVertices ( _rows * _columns );

    // Make the draw elements.
    osg::ref_ptr<osg::DrawElementsUShort> AB ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP ) );
    osg::ref_ptr<osg::DrawElementsUShort> BC ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP ) );
    osg::ref_ptr<osg::DrawElementsUShort> CD ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP ) );
    osg::ref_ptr<osg::DrawElementsUShort> DA ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP ) );

    // Loop through all the columns.
    for ( unsigned int j = 0; j < _columns; ++j )
    {
      AB->push_back ( ( ( ( _rows - 1 ) ) * _columns ) + j + numVertices );
      AB->push_back ( ( ( ( _rows - 1 ) ) * _columns ) + j               );

      CD->push_back ( j + numVertices );
      CD->push_back ( j               );
    }

    // Loop through all the rows.
    for ( unsigned int i = 0; i < _rows; ++i )
    {
      BC->push_back ( ( i * _columns ) + ( _columns - 1 ) + numVertices );
      BC->push_back ( ( i * _columns ) + ( _columns - 1 )               );

      DA->push_back ( ( i * _columns ) + numVertices );
      DA->push_back ( ( i * _columns )               );
    }

    osg::Geometry::PrimitiveSetList primSetList;
    primSetList.push_back ( AB.get() );
    primSetList.push_back ( BC.get() );
    primSetList.push_back ( CD.get() );
    primSetList.push_back ( DA.get() );

    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    // Set the points.
    geometry->setVertexArray ( points.get() );
    
    // Set the normals.
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    
    // Set the texture coordinates.
    geometry->setTexCoordArray ( 0, texCoords.get() );
    
    // Set the primitive-set list.
    geometry->setPrimitiveSetList ( primSetList );
        
    // Use vertex buffers.
    geometry->setUseDisplayList ( false );
    geometry->setUseVertexBufferObjects ( true );

    // Add the drawable.
    skirts.addDrawable ( geometry.get() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reference.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  inline unsigned int getVectorIndex ( unsigned int row, unsigned int column, unsigned int numColumns )
  {
    return ( ( row * numColumns ) + column );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::buildMesh ( const Body& body, 
                       const Extents& extents, 
                       ImagePtr elevation, 
                       const osg::Vec2d& uRange,
                       const osg::Vec2d& vRange,
                       osg::BoundingSphere& boundingSphere,
                       Vector& offset )
{
  const unsigned int rows ( _rows );
  const unsigned int columns ( _columns );

  // The number of expected bytes for our elevation data.
  const unsigned int expectedBytes ( rows * columns * sizeof ( float ) );

  // Check to make sure our elevation data is valid.
  const bool elevationValid ( elevation.valid() && 
                              GL_FLOAT == elevation->getDataType() && 
                              expectedBytes == elevation->getImageSizeInBytes() &&
                              static_cast<int> ( rows ) == elevation->s() && 
                              static_cast<int> ( columns ) == elevation->t() );

  // Shortcuts.
  const Extents::Vertex &mn ( extents.minimum() );
  const Extents::Vertex &mx ( extents.maximum() );
  const double deltaU ( uRange[1] - uRange[0] );
  const double deltaV ( vRange[1] - vRange[0] );

  for ( int i = rows - 1; i >= 0; --i )
  {
    const double u ( 1.0 - static_cast<double> ( i ) / ( rows - 1 ) );
    for ( unsigned int j = 0; j < columns; ++j )
    {
      const double v ( static_cast<double> ( j ) / ( columns - 1 ) );
      
      const double lon ( mn[0] + u * ( mx[0] - mn[0] ) );
      const double lat ( mn[1] + v * ( mx[1] - mn[1] ) );

      // The osg::Image stores it's data as char*.  However, in this case the data of the image is float.
      // The data function will calculate and return the pointer to the beginning of the float.  The pointer needs to be cast to a float pointer so the proper value is accessed.
      const double heightAboveSeaLevel ( ( elevationValid ? ( *reinterpret_cast < const float * > ( elevation->data ( rows - i - 1, j ) ) ) : 0.0 ) );

      // Calculate texture coordinate.  Lower left corner should be (0,0).
      const float s ( static_cast<float> ( uRange[0] + ( u * deltaU ) ) );
      const float t ( ( vRange[0] + ( v * deltaV ) ) );

      // Set the data.
      this->_setLocationData ( body, boundingSphere, i, j, lat, lon, heightAboveSeaLevel, s, t );
    }
  }

  // Move mesh vertices to local origin.
  offset = _points.at ( rows * columns );
  for ( Vectors::iterator iter = _points.begin(); iter != _points.end(); ++iter )
  {
    Vector &p ( *iter );
    p = p - offset;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the location data.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::_setLocationData ( const Body& body, osg::BoundingSphere& boundingSphere, unsigned int i, unsigned int j, double lat, double lon, double elevation, double s, double t )
{
  // Get the index into the vectors.
  const Vectors::size_type index ( Detail::getVectorIndex ( i, j, _columns ) );

  // Get the number of vertices.
  const Vectors::size_type numVertices ( _rows * _columns );

  // Convert lat-lon coordinates to xyz.
  Vector &p ( _points.at ( index ) );
  body.latLonHeightToXYZ ( lat, lon, elevation, p );

  // Expand the bounding sphere by the point.
  boundingSphere.expandBy ( p );

  // Assign normal vectors.
  Vector &n ( _normals.at ( index ) );
  n = p; // Minus the center, which is (0,0,0).
  n.normalize();

  // Assign texture coordinate.
  _texCoords.at ( index ).set ( Usul::Math::clamp<float> ( s, 0.0f, 1.0f ), Usul::Math::clamp<float> ( t, 0.0f, 1.0f ) );

  // Handle the skirt points.
  Vector &pSkirt ( _points.at ( index + numVertices ) );
  body.latLonHeightToXYZ ( lat, lon, elevation - _skirtHeight, pSkirt );

  // Expand the bounding sphere by the point.
  boundingSphere.expandBy ( pSkirt );
  
  // Set the normal.
  _normals.at ( index + numVertices ) = n;

  // Set the texture coordinate.
  _texCoords.at ( index + numVertices ) = _texCoords.at ( index );
}
