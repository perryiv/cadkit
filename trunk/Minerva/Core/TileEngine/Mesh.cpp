
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

#include "Usul/Algorithms/TriStrip.h"
#include "Usul/Math/Barycentric.h"
#include "Usul/Predicates/Tolerance.h"

#include "OsgTools/Convert.h"
#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"

#include "osg/BlendFunc"
#include "osg/Geode"
#include "osg/Depth"
#include "osg/Hint"
#include "osg/PolygonOffset"

using namespace Minerva::Core::TileEngine;


///////////////////////////////////////////////////////////////////////////////
//
//  Polygon offset constants.
//
///////////////////////////////////////////////////////////////////////////////

const float MESH_POLYGON_OFFSET_FACTOR  (  0.0f );
const float MESH_POLYGON_OFFSET_UNITS   ( 14.0f );
const float SKIRT_POLYGON_OFFSET_FACTOR (  0.0f );
const float SKIRT_POLYGON_OFFSET_UNITS  ( 16.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Render bin constants.
//
///////////////////////////////////////////////////////////////////////////////

const int MESH_RENDER_BIN ( 0 );
const int SKIRT_RENDER_BIN ( 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::Mesh ( unsigned int rows, unsigned int columns, double skirtHeight, const Extents& extents ) :
  _latLonPoints ( rows * columns * 2 ),
  _points    ( rows * columns * 2 ),
  _normals   ( rows * columns * 2 ),
  _texCoords ( rows * columns * 2 ),
  _meshPrimitives(),
  _rows      ( rows ),
  _columns   ( columns ),
  _skirtHeight ( skirtHeight ),
  _extents ( extents ),
  _borders ( new osg::Group ),
  _skirts ( new osg::Geode ),
  _ground ( new osg::Geode ),
  _lowerLeft(),
  _boundingSphere()
{
  // Make the draw elements now because they are only dependent on the rows and columns.
  // There is one tri-strip for each adjacent pair of rows.
  Usul::Algorithms::triStripIndices ( _rows, _columns, _meshPrimitives );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::const_reference Mesh::_point ( size_type r, size_type c ) const
{
  return _points.at ( this->_index ( r, c ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::_buildGeometry ( osg::Geode& mesh, osg::Geode& skirts ) const
{
  // Allocate the points.
  osg::ref_ptr<osg::Vec3Array> points  ( new osg::Vec3Array (  _points.begin(), _points.end() ) );
  
  // Allocate the normals.
  osg::ref_ptr<osg::Vec3Array> normals ( new osg::Vec3Array ( _normals.begin(), _normals.end()  ) );
  
  // Allocation texture coordinates.
  osg::ref_ptr<osg::Vec2Array> texCoords ( new osg::Vec2Array ( _texCoords.begin(), _texCoords.end() ) );

  // Make the main mesh.
  {
    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );
    
    // Set the points.
    geometry->setVertexArray ( points.get() );
    
    // Set the normals.
    geometry->setNormalArray ( normals.get() );
    geometry->setNormalBinding ( osg::Geometry::BIND_PER_VERTEX );
    
    // Set the texture coordinates.
    geometry->setTexCoordArray ( 0, texCoords.get() );

    // Make the draw elements.
    osg::Geometry::PrimitiveSetList primitives ( _meshPrimitives.size() );

    for ( unsigned int i = 0; i < primitives.size(); ++i )
    {
      const Indices& indices ( _meshPrimitives.at ( i ) );

      // Create the draw elements.
      osg::ref_ptr<osg::DrawElementsUShort> drawElements ( new osg::DrawElementsUShort ( osg::PrimitiveSet::TRIANGLE_STRIP, indices.size(), &indices[0] ) );

      // Assign the value.
      primitives[i] = drawElements.get();
    }
    
    // Set the primitive-set list.
    geometry->setPrimitiveSetList ( primitives );
        
    // Use vertex buffers.
    geometry->setUseDisplayList ( false );
    geometry->setUseVertexBufferObjects ( true );

    // Add the drawable.
    mesh.addDrawable ( geometry.get() );

    // Get the state set.
    osg::ref_ptr<osg::StateSet> ss ( mesh.getOrCreateStateSet() );

    // Turn off back-face culling.
    ss->setMode ( GL_CULL_FACE, osg::StateAttribute::OFF );

    osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
    offset->setFactor ( MESH_POLYGON_OFFSET_FACTOR );
    offset->setUnits  ( MESH_POLYGON_OFFSET_UNITS  );
    ss->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

    if ( 0 != MESH_RENDER_BIN )
      ss->setRenderBinDetails( MESH_RENDER_BIN, "RenderBin" );
  }

  // Make the skirts.
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

    // Get the state set.
    osg::ref_ptr<osg::StateSet> ss ( skirts.getOrCreateStateSet() );

    osg::ref_ptr<osg::PolygonOffset> offset ( new osg::PolygonOffset );
    offset->setFactor ( SKIRT_POLYGON_OFFSET_FACTOR );
    offset->setUnits  ( SKIRT_POLYGON_OFFSET_UNITS );
    ss->setAttributeAndModes ( offset.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

    if ( 0 != SKIRT_RENDER_BIN )
      ss->setRenderBinDetails( SKIRT_RENDER_BIN, "RenderBin" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the mesh.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Mesh::buildMesh ( const Body& body, 
                             ElevationDataPtr elevation, 
                             const osg::Vec2d& uRange,
                             const osg::Vec2d& vRange,
                             osg::BoundingSphere& boundingSphere )
{
  Extents extents ( _extents );

  const unsigned int rows ( _rows );
  const unsigned int columns ( _columns );

  // Check to make sure our elevation data is valid.
  const bool elevationValid ( elevation.valid() && 
                              rows == elevation->height() && 
                              columns == elevation->width() );

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

      const double heightAboveSeaLevel ( elevationValid ? elevation->value ( rows - i - 1, j ) : 0.0 );

      // Calculate texture coordinate.  Lower left corner should be (0,0).
      const float s ( static_cast<float> ( uRange[0] + ( u * deltaU ) ) );
      const float t ( ( vRange[0] + ( v * deltaV ) ) );

      // Set the data.
      this->_setLocationData ( body, boundingSphere, i, j, lat, lon, heightAboveSeaLevel, s, t );
    }
  }

  // Move mesh vertices to local origin.
  // The offset is the first point of the skirts.
  Vector offset ( _points.at ( rows * columns ) );
  for ( Vectors::iterator iter = _points.begin(); iter != _points.end(); ++iter )
  {
    Vector &p ( *iter );
    p = p - offset;
  }

  // Make group to hold the meshes.
  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( offset ) );

  // Make the geodes.
  osg::ref_ptr<osg::Geode> ground ( new osg::Geode );
  osg::ref_ptr<osg::Geode> skirts ( new osg::Geode );

  // Build the scene for the mesh.
  this->_buildGeometry ( *ground, *skirts );

  // Add to the matrix transform.
  mt->addChild ( skirts.get() );
  mt->addChild ( ground.get() );

  // Add the place-holder for the border.
  mt->addChild ( _borders.get() );

  // Set needed variables.
  _lowerLeft = offset;
  _skirts = skirts.get();
  _ground = ground.get();
  _boundingSphere = boundingSphere;

  // Return the MatrixTransform.
  return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the location data.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::_setLocationData ( const Body& body, osg::BoundingSphere& boundingSphere, unsigned int i, unsigned int j, double lat, double lon, double elevation, double s, double t )
{
  // Get the index into the vectors.
  const Vectors::size_type index ( this->_index ( i, j ) );

  // Get the number of vertices.
  const Vectors::size_type numVertices ( _rows * _columns );

  // Convert lat-lon coordinates to xyz.
  Vector &p ( _points.at ( index ) );
  body.latLonHeightToXYZ ( lat, lon, elevation, p );
  
  // Save the lat/lon value.  
  // This value needs to be saved because going from x,y,z to lat,lon,height will not give us the same value due to inaccuracies in the conversion.
  _latLonPoints.at ( index ) = Vertex ( lon, lat, elevation );

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


///////////////////////////////////////////////////////////////////////////////
//
//  Build line-segment for the border.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* Mesh::_buildBorder() const
{
  // Make geode to hold the geometry.
  osg::ref_ptr<osg::Geode> geode ( new osg::Geode );

  // Make the colors.
  osg::Vec4f color ( 1.0f, 0.0f, 0.0f, 1.0f );
  osg::ref_ptr<osg::Vec4Array> horizColors ( new osg::Vec4Array );
  osg::ref_ptr<osg::Vec4Array> vertColors  ( new osg::Vec4Array );
  horizColors->resize ( this->columns(), color );
  vertColors->resize  ( this->rows(),    color );

  // Make the vertices.
  osg::ref_ptr<osg::Vec3Array> top    ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> bottom ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> left   ( new osg::Vec3Array );
  osg::ref_ptr<osg::Vec3Array> right  ( new osg::Vec3Array );

  // Make space.
  top->reserve    ( this->columns() );
  bottom->reserve ( this->columns() );
  left->reserve   ( this->rows() );
  right->reserve  ( this->rows() );

  // Loop through the columns.
  for ( unsigned int j = 0; j < this->columns(); ++j )
  {
    top->push_back    ( this->_point (                0, j ) );
    bottom->push_back ( this->_point ( this->rows() - 1, j ) );
  }

  // Loop through the rows.
  for ( unsigned int i = 0; i < this->rows(); ++i )
  {
    left->push_back  ( this->_point ( i, 0 ) );
    right->push_back ( this->_point ( i, this->columns() - 1 ) );
  }

  // Top
  {
    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    // Add geometry properties.
    geometry->setVertexArray ( top.get() );
    geometry->setColorArray ( horizColors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray ( 0x0 );
    geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

    // Add geometry to geode.
    geode->addDrawable ( geometry.get() );
  }

  // Bottom
  {
    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    // Add geometry properties.
    geometry->setVertexArray ( bottom.get() );
    geometry->setColorArray ( horizColors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray ( 0x0 );
    geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

    // Add geometry to geode.
    geode->addDrawable ( geometry.get() );
  }

  // Left
  {
    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    // Add geometry properties.
    geometry->setVertexArray ( left.get() );
    geometry->setColorArray ( vertColors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray ( 0x0 );
    geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

    // Add geometry to geode.
    geode->addDrawable ( geometry.get() );
  }

  // Right
  {
    // Make the geometry.
    osg::ref_ptr<osg::Geometry> geometry ( new osg::Geometry );

    // Add geometry properties.
    geometry->setVertexArray ( right.get() );
    geometry->setColorArray ( vertColors.get() );
    geometry->setColorBinding ( osg::Geometry::BIND_PER_VERTEX );
    geometry->setNormalArray ( 0x0 );
    geometry->setNormalBinding ( osg::Geometry::BIND_OFF );
    geometry->addPrimitiveSet ( new osg::DrawArrays ( osg::PrimitiveSet::LINE_STRIP, 0, geometry->getVertexArray()->getNumElements() ) );

    // Add geometry to geode.
    geode->addDrawable ( geometry.get() );
  }

  // Set properties.
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 3.0f );
  OsgTools::State::StateSet::setPolygonsTextures ( geode->getOrCreateStateSet(), false );
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  // Get the state set.
  osg::ref_ptr<osg::StateSet > ss ( geode->getOrCreateStateSet() );
  
  // Set depth parameters.
  osg::ref_ptr<osg::Depth> depth ( new osg::Depth ( osg::Depth::LEQUAL, 0.0, 1.0, false ) );
  ss->setAttributeAndModes ( depth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  // Set the line parameters.
  ss->setMode ( GL_LINE_SMOOTH, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  ss->setMode ( GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
  
  // Add a blend function.
  osg::ref_ptr<osg::BlendFunc> blend ( new osg::BlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ) );
  ss->setAttributeAndModes ( blend.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );
 
  // Set the hint.
  osg::ref_ptr<osg::Hint> hint ( new osg::Hint ( GL_LINE_SMOOTH_HINT, GL_NICEST ) );
  ss->setAttributeAndModes ( hint.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON );

  // Draw after the tile.
  ss->setRenderBinDetails ( 2, "RenderBin" );

  // Return new geode.
  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the smallest distance (squared) from the given point.
//
///////////////////////////////////////////////////////////////////////////////

double Mesh::getSmallestDistanceSquared ( const osg::Vec3d& point ) const
{
  osg::Vec3d p00 ( _lowerLeft + this->_point ( 0, 0 ) );
  osg::Vec3d p0N ( _lowerLeft + this->_point ( 0, this->columns() - 1 ) );
  osg::Vec3d pN0 ( _lowerLeft + this->_point ( this->rows() - 1, 0 ) );
  osg::Vec3d pNN ( _lowerLeft + this->_point ( this->rows() - 1, this->columns() - 1 ) );
  const osg::Vec3d& pBC ( _boundingSphere.center() );
  
  // Squared distances from the eye to the points.
  const double dist00 ( ( point - p00 ).length2() );
  const double dist0N ( ( point - p0N ).length2() );
  const double distN0 ( ( point - pN0 ).length2() );
  const double distNN ( ( point - pNN ).length2() );
  const double distBC ( ( point - pBC ).length2() );

  // Check with smallest distance.
  const double dist ( Usul::Math::minimum ( dist00, dist0N, distN0, distNN, distBC ) );

  // Return the distance.
  return dist;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the border.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::showBorder ( bool show )
{
  //_borders->setNodeMask ( ( show ? 0xffffffff : 0x0 ) );

  if ( show )
  {
    OsgTools::Group::removeAllChildren ( _borders.get() );
    _borders->addChild ( this->_buildBorder() );
  }
  else
  {
    OsgTools::Group::removeAllChildren ( _borders.get() );
  }

}


///////////////////////////////////////////////////////////////////////////////
//
//  Show the skirts.
//
///////////////////////////////////////////////////////////////////////////////

void Mesh::showSkirts ( bool show )
{
  _skirts->setNodeMask  ( show ? 0xffffffff : 0x0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the elevation value from the triangles at a given lat,lon.
//
///////////////////////////////////////////////////////////////////////////////

double Mesh::elevation ( double lat, double lon, const LandModel& land ) const
{
  if ( false == _extents.contains ( Extents::Vertex ( lon, lat ) ) )
  {
    return 0.0;
  }
  
  // Shortcuts.
  Extents e ( _extents );
  const Extents::Vertex &mn ( e.minimum() );
  const Extents::Vertex &mx ( e.maximum() );

  const double u ( ( lon - mn[0] ) / ( mx[0] - mn[0] ) );

  // How many tri-strips there are.
  const unsigned int numStrips ( _meshPrimitives.size() );

  // Get the index.
  const unsigned int index ( numStrips * u );

  // Get the row of the tri-strip we need.
  const unsigned int row ( index == numStrips ? 0 : numStrips - index - 1 );

  // Get the tri-strip.
  const Indices& triStrip ( _meshPrimitives.at ( row) );

  // Loop over the triangles of the tristrip.
  for ( unsigned int i = 0; i < triStrip.size() - 2; ++i )
  {
    // Get the 3 points of the current triangle in lat-lon coordinates.
    Vertex t0 ( _latLonPoints.at ( triStrip.at ( i ) ) );
    Vertex t1 ( _latLonPoints.at ( triStrip.at ( i + 1 ) ) );
    Vertex t2 ( _latLonPoints.at ( triStrip.at ( i + 2 ) ) );

    // Weighted position.
    Vertex weights;

    // See if the lat/lon is inside this triangle.
    if ( Mesh::_containsPoint ( t0, t1, t2, Vertex ( lon, lat, 0.0 ), weights ) )
    {
      const double elevation ( weights[0] * t0[2] + weights[1] * t1[2] + weights[2] * t2[2] );
      return elevation;
    }
  }

  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  See if the value is between 0 and 1.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  bool isValueBetweenZeroAndOne ( double value )
  {
    Usul::Predicates::Tolerance<double> pred ( 1e-10 );

    // First handle cases where the value is close to zero or one.
    const bool isCloseToZero ( pred ( 0.0, value ) );
    if ( isCloseToZero )
      return true;

    const bool isCloseToOne ( pred ( 1.0, value ) );
    if ( isCloseToOne )
      return true;

    return 0.0 <= value && value <= 1.0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the point contained in the triangle?
//
///////////////////////////////////////////////////////////////////////////////

bool Mesh::_containsPoint ( const Vertex& v0, const Vertex& v1, const Vertex& v2, const Vertex& point, Vertex& weights )
{
  Usul::Math::Vec3d t0 ( Usul::Convert::Type<Vertex,Usul::Math::Vec3d>::convert ( v0 ) );
  Usul::Math::Vec3d t1 ( Usul::Convert::Type<Vertex,Usul::Math::Vec3d>::convert ( v1 ) );
  Usul::Math::Vec3d t2 ( Usul::Convert::Type<Vertex,Usul::Math::Vec3d>::convert ( v2 ) );
  Usul::Math::Vec3d p  ( Usul::Convert::Type<Vertex,Usul::Math::Vec3d>::convert ( point ) );

  // Zero out the elevation, so the test is only in the lat/long plane.
  t0[2] = 0; t1[2] = 0; t2[2] = 0; p[2] = 0;

  Usul::Math::Vec3d barycentricCoords ( Usul::Math::barycentric ( t0, t1, t2, p ) );

  // See if the all the values are between zero and one.  This means that the point is inside the triangle.
  if ( Detail::isValueBetweenZeroAndOne ( barycentricCoords[0] ) &&
       Detail::isValueBetweenZeroAndOne ( barycentricCoords[1] ) &&
       Detail::isValueBetweenZeroAndOne ( barycentricCoords[2] ) )
  {
    weights.set ( barycentricCoords[0], barycentricCoords[1], barycentricCoords[2] );
    return true;
  }

  return false;
}
