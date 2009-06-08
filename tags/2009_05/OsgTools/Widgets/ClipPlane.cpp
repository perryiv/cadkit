
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Widgets/ClipPlane.h"

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

ClipPlane::ClipPlane ( const osg::BoundingBox& bb, osg::ClipPlane *clip ) : BaseClass(),
_bb ( bb ),
_clipPlane ( clip )
{
  this->_setGeometry();
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
//  Set the geometry.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlane::_setGeometry()
{
  // Remove what we have.
  this->removeChild ( 0, this->getNumChildren() );

  // Get the clip plane.
  osg::Plane plane ( _clipPlane->getClipPlane() );

  // Geode
  osg::ref_ptr< osg::Geode > geode ( new osg::Geode );

  // Make the geometry
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );
  //geode->addDrawable ( geometry.get() );
  
  // Make the vertices
  osg::ref_ptr < osg::Vec3Array > vertices ( new osg::Vec3Array );
  OsgTools::Utilities::boundingBoxIntersect ( plane, _bb, *vertices );

  // Make the normals
  osg::ref_ptr < osg::Vec3Array > normals ( new osg::Vec3Array(1) );
  normals->at( 0 ) = plane.getNormal();

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
  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, vertices->size() ) );

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
  edge->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::LINE_LOOP, 0, vertices->size() ) );

  // Add the geometry
  geode->addDrawable( edge.get() );

  // Set the geometrys
  this->addChild ( geode.get() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void ClipPlane::update()
{
  this->_setGeometry();
}
