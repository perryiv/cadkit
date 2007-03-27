
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "ClipPlane.h"

#include "OsgTools/Draggers/Translate1.h"
#include "OsgTools/Draggers/Translate2.h"
#include "OsgTools/Draggers/Rotate1.h"
#include "OsgTools/Utilities/Intersect.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/ShapeDrawable"

using namespace OsgTools::Widgets;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlane::ClipPlane ( const osg::BoundingBox& bb, osg::ClipPlane *clip ) :
_bb ( bb ),
_clipPlane ( clip )
{
  // Get the clip plane.
  osg::Plane plane ( _clipPlane->getClipPlane() );

  osg::Vec3 normal ( plane.getNormal () );
  double distance ( plane.distance( osg::Vec3 ( 0.0, 0.0, 0.0 ) ) );
  osg::Vec3 v0, v1, v2, v3;

  osg::Vec3 top_left     ( bb.corner( 1 ) );
  osg::Vec3 bottom_left  ( bb.corner( 0 ) );
  osg::Vec3 bottom_right ( bb.corner( 6 ) );
  osg::Vec3 top_right    ( bb.corner( 7 ) );

  // Calculate v0.
  v0.x() = top_left.x() + distance;
  v0.y() = top_left.y() + distance;
  v0.z() = ( ( - ( normal.x() * v0.x() ) - ( normal.y() * v0.y() ) ) / normal.z() ) + distance;

  // Calculate v1.
  v1.x() = bottom_left.x() + distance;
  v1.y() = bottom_left.y() + distance;
  v1.z() = ( ( - ( normal.x() * v1.x() ) - ( normal.y() * v1.y() ) ) / normal.z() ) + distance;

  // Calculate v2.
  v2.x() = bottom_right.x() + distance;
  v2.y() = bottom_right.y() + distance;
  v2.z() = ( ( - ( normal.x() * v2.x() ) - ( normal.y() * v2.y() ) ) / normal.z() ) + distance;

  // Calculate v3.
  v3.x() = top_right.x() + distance;
  v3.y() = top_right.y() + distance;
  v3.z() = ( ( - ( normal.x() * v3.x() ) - ( normal.y() * v3.y() ) ) / normal.z() ) + distance;

  // Geode
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // Make the geometry
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  geode->addDrawable ( geometry.get() );
  
  // Make the vertices
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array(4) );
  //vertices->at( 0 ) = v0;
  //vertices->at( 1 ) = v1;
  //vertices->at( 2 ) = v2;
  //vertices->at( 3 ) = v3;
  OsgTools::Utilities::boundingBoxIntersect ( plane, bb, *vertices );

  // Make the normals
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array(1) );
  normals->at( 0 ) = normal;

  // Make the colors
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array(1) );
  colors->at ( 0 ).set(1.0f,1.0f,1.0f,0.1f);

  // Set the arrays
  geometry->setVertexArray ( vertices.get() );
  geometry->setNormalArray ( normals.get() );
  geometry->setColorArray  ( colors.get() );

  // Set the bindings
  geometry->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  geometry->setColorBinding  ( osg::Geometry::BIND_OVERALL );

  // Add the primitive set
  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, 4 ) );

  // Get or Create the state set
  osg::ref_ptr < osg::StateSet > stateset ( geometry->getOrCreateStateSet() );

  // Set needed states from transparency
  stateset->setMode ( GL_BLEND,osg::StateAttribute::ON);
  stateset->setMode ( GL_DEPTH_TEST, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  // Make a geometry for the edge of plane
  osg::ref_ptr< osg::Geometry > edge ( new osg::Geometry );

  // Make the colors
  osg::ref_ptr < osg::Vec4Array > eColors ( new osg::Vec4Array(1) );
  
  // Set the value
  eColors->at ( 0 ).set(0.0f,0.0f,0.0f,1.0f);

  // Set the arrays
  edge->setVertexArray ( vertices.get() );
  edge->setNormalArray ( normals.get() );
  edge->setColorArray  ( eColors.get() );

  // Set the bindings
  edge->setNormalBinding ( osg::Geometry::BIND_OVERALL );
  edge->setColorBinding  ( osg::Geometry::BIND_OVERALL );

  // Add the primitive set
  edge->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, 4 ) );

  // Add the geometry
  geode->addDrawable( edge.get() );

  // Make geode for dragging
  osg::ref_ptr< osg::Geode > draggedGeode ( new osg::Geode );

  // Add the same quad as normal geode
  draggedGeode->addDrawable( reinterpret_cast < osg::Geometry* > ( geometry->clone( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Make a geometry for the edge of plane while dragging
  osg::ref_ptr< osg::Geometry > redEdge ( reinterpret_cast < osg::Geometry* > ( edge->clone( osg::CopyOp::DEEP_COPY_ALL ) ) );

  // Make color array for red color
  osg::ref_ptr< osg::Vec4Array > redColors ( new osg::Vec4Array ( 1 ) );

  // Set the color value
  redColors->at( 0 ).set ( 1.0, 0.0, 0.0, 1.0 );

  // Set the color array
  redEdge->setColorArray  ( redColors.get() );

  // Add the red edge to dragged geode
  draggedGeode->addDrawable( redEdge.get() );

  typedef OsgTools::Draggers::Translate1 Dragger;

  // Make a dragger
  osg::ref_ptr< Dragger > dragger ( new Dragger ( Dragger::Z ) );

  // Set the geometrys
  dragger->geometry( Dragger::NORMAL, geode.get() );
  dragger->geometry( Dragger::DRAGGING, draggedGeode.get() );

  // Make the callback
  osg::ref_ptr< ClipPlaneCallback > callback ( new ClipPlaneCallback ( v0, v1, v2, _clipPlane.get() ) );

  // Add the callback
  dragger->callback( Dragger::MOVE, callback.get() );

  typedef OsgTools::Draggers::Rotate1 Rotate;

  // Make a dragger to rotate along the x axis
  osg::ref_ptr< Rotate > rx ( new Rotate ( osg::Vec3 ( 1.0, 0.0, 0.0 ) ) );

  osg::ref_ptr<osg::Sphere> sx ( new osg::Sphere );
  
  const double xMid ( ( bb.xMax() - bb.xMin() ) / 2);

  sx->setCenter ( osg::Vec3 ( bb.xMin() + xMid, bb.yMax(), bb.zMax() ) );
  sx->setRadius ( 0.1f );
  osg::ref_ptr<osg::ShapeDrawable> sdx ( new osg::ShapeDrawable ( sx.get() ) );

  osg::ref_ptr < osg::Geode > gx ( new osg::Geode );

  gx->addDrawable ( sdx.get() );

  rx->geometry( Dragger::NORMAL, gx.get() );
  rx->geometry( Dragger::DRAGGING, gx.get() );

  // Make a dragger to rotate along the y axis
  osg::ref_ptr< Rotate > ry ( new Rotate ( osg::Vec3 ( 0.0, 1.0, 0.0 ) ) );

  osg::ref_ptr<osg::Sphere> sy ( new osg::Sphere );

  const double yMid ( ( bb.yMax() - bb.yMin() ) / 2);
  const double zMid ( ( bb.zMax() - bb.zMin() ) / 2);
  
  sy->setCenter ( osg::Vec3 ( bb.xMax(), bb.yMin() + yMid, bb.zMin() + zMid ) );
  sy->setRadius ( 0.1f );
  osg::ref_ptr<osg::ShapeDrawable> sdy ( new osg::ShapeDrawable ( sy.get() ) );

  osg::ref_ptr < osg::Geode > gy ( new osg::Geode );

  gy->addDrawable ( sdy.get() );

  ry->geometry( Dragger::NORMAL, gy.get() );
  ry->geometry( Dragger::DRAGGING, gy.get() );

  // Group for the three draggers
  osg::ref_ptr < osg::Group > draggers ( new osg::Group );

  // Add the draggers to the scene
  draggers->addChild( rx.get() );
  draggers->addChild( ry.get() );
  draggers->addChild( dragger.get() );

  this->geometry( Dragger::NORMAL, draggers.get() );
  this->geometry( Dragger::DRAGGING, draggers.get() );

}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ClipPlane::~ClipPlane()
{
}



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

ClipPlane::ClipPlaneCallback::ClipPlaneCallback ( const osg::Vec3& topLeft, const osg::Vec3& bottomLeft, const osg::Vec3& topRight, osg::ClipPlane* clipPlane ) :
BaseClass(),
_topLeft( topLeft ),
_bottomLeft ( bottomLeft ),
_topRight ( topRight ),
_clipPlane ( clipPlane )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Copy Constructor
//
///////////////////////////////////////////////////////////////////////////////

ClipPlane::ClipPlaneCallback::ClipPlaneCallback ( const ClipPlaneCallback &c ) :
BaseClass( c ),
_topLeft( c._topLeft ),
_bottomLeft ( c._bottomLeft ),
_topRight ( c._topRight ),
_clipPlane ( c._clipPlane )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

ClipPlane::ClipPlaneCallback::~ClipPlaneCallback()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the callback.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlane::ClipPlaneCallback::operator() ( Dragger &d )
{
  osg::Matrix m ( d.getMatrix() );

  osg::Vec3 topLeft    ( _topLeft * m );
  osg::Vec3 bottomLeft ( _bottomLeft * m );
  osg::Vec3 topRight   ( _topRight * m );

  osg::Plane plane( topLeft, bottomLeft, topRight );

  _clipPlane->setClipPlane( plane );
}

