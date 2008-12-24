
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

#include "OsgTools/Group.h"
#include "OsgTools/State/StateSet.h"

#include "osg/BlendFunc"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Depth"
#include "osg/Hint"
#include "osg/PolygonOffset"

#include "osgUtil/LineSegmentIntersector"

using namespace Minerva::Core::TileEngine;


///////////////////////////////////////////////////////////////////////////////
//
//  Polygon offset constants.
//
///////////////////////////////////////////////////////////////////////////////

const float MESH_POLYGON_OFFSET_FACTOR ( 0.0f );
const float MESH_POLYGON_OFFSET_UNITS  ( 14.0f );
const float SKIRT_POLYGON_OFFSET_FACTOR ( 0.0f );
const float SKIRT_POLYGON_OFFSET_UNITS  ( 16.0f );


///////////////////////////////////////////////////////////////////////////////
//
//  Render bin constants.
//
///////////////////////////////////////////////////////////////////////////////

const unsigned int MESH_RENDER_BIN ( 0 );
const unsigned int SKIRT_RENDER_BIN ( 0 );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::Mesh ( unsigned int rows, unsigned int columns, double skirtHeight, const Extents& extents ) :
  _points    ( rows * columns * 2 ),
  _normals   ( rows * columns * 2 ),
  _texCoords ( rows * columns * 2 ),
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
}


///////////////////////////////////////////////////////////////////////////////
//
//  Access to a single point.
//
///////////////////////////////////////////////////////////////////////////////

Mesh::const_reference Mesh::point ( size_type r, size_type c ) const
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
                             ImagePtr elevation, 
                             const osg::Vec2d& uRange,
                             const osg::Vec2d& vRange,
                             osg::BoundingSphere& boundingSphere )
{
  Extents extents ( _extents );

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
    top->push_back    ( this->point (                0, j ) );
    bottom->push_back ( this->point ( this->rows() - 1, j ) );
  }

  // Loop through the rows.
  for ( unsigned int i = 0; i < this->rows(); ++i )
  {
    left->push_back  ( this->point ( i, 0 ) );
    right->push_back ( this->point ( i, this->columns() - 1 ) );
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
  const osg::Vec3d &p00 ( _lowerLeft + this->point ( 0, 0 ) );
  const osg::Vec3d &p0N ( _lowerLeft + this->point ( 0, this->columns() - 1 ) );
  const osg::Vec3d &pN0 ( _lowerLeft + this->point ( this->rows() - 1, 0 ) );
  const osg::Vec3d &pNN ( _lowerLeft + this->point ( this->rows() - 1, this->columns() - 1 ) );
  const osg::Vec3d &pBC ( _boundingSphere.center() );
  
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
//  Intersect with line segment defined by two points.
//
///////////////////////////////////////////////////////////////////////////////

bool Mesh::intersect ( const osg::Vec3d& point0, const osg::Vec3d& point1, osg::Vec3d& point )
{
  // Points to intersect with.
  osg::Vec3d pt0 ( point0 - _lowerLeft );
  osg::Vec3d pt1 ( point1 - _lowerLeft );
  
  // Make the intersector.
  typedef osgUtil::LineSegmentIntersector Intersector;
  osg::ref_ptr<Intersector> intersector ( new Intersector ( pt0, pt1 ) );
  
  // Declare the pick-visitor.
  typedef osgUtil::IntersectionVisitor Visitor;
  osg::ref_ptr<Visitor> visitor ( new Visitor );
  visitor->setIntersector ( intersector.get() );

  // Return zero if no node.
  if ( false == _ground.valid() )
    return false;

  // Intersect the scene.
  _ground->accept ( *visitor );

  // Get the hit-list for our line-segment.
  typedef osgUtil::LineSegmentIntersector::Intersections Intersections;
  const Intersections &hits = intersector->getIntersections();
  if ( hits.empty() )
    return false;
  
  // Set the hit.
  point = intersector->getFirstIntersection().getWorldIntersectPoint();
  point += _lowerLeft;

  return true;
}
