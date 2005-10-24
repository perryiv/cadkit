///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Mike Jackson
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////


#include "OsgTools/GlassBoundingBox.h"
#include <algorithm>
#include <functional>

#include "OsgTools/State/StateSet.h"
#include "OsgTools/SortBackToFrontCallback.h"
#include "OsgTools/Font.h"

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec4"
#include "osg/LineWidth"
#include "osg/StateSet"
#include "osg/AlphaFunc"
#include "osg/StateSet"

#include "osgText/Text"

#include <limits>
#include <iostream>

using namespace OsgTools;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox() : BaseClass(),
_xMax (std::numeric_limits< float >::min()),
_xMin (std::numeric_limits< float >::max()),
_yMax (std::numeric_limits< float >::min()),
_yMin (std::numeric_limits< float >::max()),
_zMax (std::numeric_limits< float >::min()),
_zMin (std::numeric_limits< float >::max())
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox(
        const float xMn, const float yMn, float const zMn,
        const float xMx, const float yMx, float const zMx): BaseClass()

{
  _xMax = xMx;
  _xMin = xMn;
  _yMax = yMx;
  _yMin = yMn;
  _zMax = zMx;
  _zMin = zMn;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

GlassBoundingBox::GlassBoundingBox( const osg::BoundingBox& bb ) : 
BaseClass(),
_xMax ( bb.xMax() ),
_xMin ( bb.xMin() ),
_yMax ( bb.yMax() ),
_yMin ( bb.yMin() ),
_zMax ( bb.zMax() ),
_zMin ( bb.zMin() )
{
  
}

///////////////////////////////////////////////////////////////////////////////
//
// Operator () overload
//
///////////////////////////////////////////////////////////////////////////////

void GlassBoundingBox::operator() ( osg::Group *root, bool outline, bool glass, bool numbers ) 
{
  osg::BoundingBox bb( _xMin,_yMin,_zMin,_xMax,_yMax,_zMax );

  // Make the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  
  // Fill the vertices
  for( unsigned int i = 0; i < 8; ++i )
    vertices->push_back ( bb.corner( i ) );
  
  

  if( outline )
    root->addChild ( this->_makeBoxOutline ( *vertices ) );

  if( glass )
  {
    root->addChild ( this->_makeBoxGlass ( *vertices ) );
  }

  // Add numbers to the bounding box.  This is mainly for debugging.
  if( numbers )
    root->addChild( this->_makeNumbers() );
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the "Box" outline
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_makeBoxOutline ( osg::Vec3Array& vertices )
{
  typedef osg::DrawElementsUInt DrawElements;
  
  osg::ref_ptr< DrawElements > lines  ( new DrawElements ( osg::PrimitiveSet::LINES, 0 ) );
  
  lines->push_back ( 0 );
  lines->push_back ( 1 );
  lines->push_back ( 0 );
  lines->push_back ( 2 );
  lines->push_back ( 0 );
  lines->push_back ( 4 );
  
  lines->push_back ( 6 );
  lines->push_back ( 2 );
  lines->push_back ( 6 );
  lines->push_back ( 4 );
  lines->push_back ( 6 );
  lines->push_back ( 7 );
  
  lines->push_back ( 3 );
  lines->push_back ( 2 );
  lines->push_back ( 3 );
  lines->push_back ( 7 );
  lines->push_back ( 3 );
  lines->push_back ( 1 );
  
  lines->push_back ( 5 );
  lines->push_back ( 7 );
  lines->push_back ( 5 );
  lines->push_back ( 1 );
  lines->push_back ( 5 );
  lines->push_back ( 4 );
    
  osg::ref_ptr < osg::Geometry > geometry ( new osg::Geometry );

  // Set the vertices.
  geometry->setVertexArray  ( &vertices );

  // Add the primitive sets.
  geometry->addPrimitiveSet ( lines.get() );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  
  colors->resize ( 12 );

  osg::Vec4 yellow (1.0f, 1.0f, 0.0f, 1.0f);
  std::fill( colors->begin(), colors->end(), yellow );
  
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );
  
  // Set the line-width.
  OsgTools::State::StateSet::setLineWidth ( geode.get(), 2.0f );

  // Turn off lighting.
  OsgTools::State::StateSet::setLighting ( geode.get(), false );
  
  geode->addDrawable ( geometry.get() );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the transparent "sides" on the bounding box
//
///////////////////////////////////////////////////////////////////////////////
 
osg::Node* GlassBoundingBox::_makeBoxGlass   ( osg::Vec3Array& vertices )
{
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );

  // Set state for transparency
  osg::ref_ptr< osg::StateSet > stateset ( geode->getOrCreateStateSet() );
  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  //stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON );
  //geode->setCullCallback( new OsgTools::SortBackToFrontCallback );

  typedef osg::DrawElementsUInt DrawElements;
  osg::ref_ptr< osg::Vec3Array > box ( new osg::Vec3Array );

  // Bottom quad. ( XZ min )
  box->push_back ( vertices.at ( 0 ) );
  box->push_back ( vertices.at ( 1 ) );
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Top quad. ( XZ max )
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 6 ) );

  // Left quad. ( YZ min )
  box->push_back ( vertices.at ( 0 ) );
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 6 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Right quad. ( YZ max )
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 1 ) );

  // Front quad. ( XY max )
  box->push_back ( vertices.at ( 6 ) );
  box->push_back ( vertices.at ( 7 ) );
  box->push_back ( vertices.at ( 5 ) );
  box->push_back ( vertices.at ( 4 ) );

  // Back quad. ( XY min )
  box->push_back ( vertices.at ( 2 ) );
  box->push_back ( vertices.at ( 3 ) );
  box->push_back ( vertices.at ( 1 ) );
  box->push_back ( vertices.at ( 0 ) );

  osg::ref_ptr< osg::Geometry > geometry ( new osg::Geometry );

  // Set the vertices.
  geometry->setVertexArray  ( box.get() );

  // Add the primitive set.
  geometry->addPrimitiveSet ( new osg::DrawArrays( osg::PrimitiveSet::QUADS, 0, box->size() ) );
  
  osg::ref_ptr < osg::Vec4Array > colors ( new osg::Vec4Array );

  const float alpha ( 0.25f );

  colors->push_back ( osg::Vec4 ( 0.20f, 0.0f,  0.0f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.20f, 0.0f,  0.0f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.05f, 0.05f, 0.05f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.05f, 0.05f, 0.05f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.0f,  0.0f,  0.20f, alpha ) );
  colors->push_back ( osg::Vec4 ( 0.0f,  0.0f,  0.20f, alpha ) );
  
  // Set colors
  geometry->setColorArray ( colors.get() );
  geometry->setColorBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  // Set the normals
  osg::ref_ptr< osg::Vec3Array > normals ( new osg::Vec3Array );

  normals->push_back( osg::Vec3 (  0.0f, -1.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  1.0f,  0.0f ) );
  normals->push_back( osg::Vec3 ( -1.0f,  0.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  1.0f,  0.0f,  0.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  0.0f,  1.0f ) );
  normals->push_back( osg::Vec3 (  0.0f,  0.0f, -1.0f ) );

  geometry->setNormalArray ( normals.get() );
  geometry->setNormalBinding ( osg::Geometry::BIND_PER_PRIMITIVE );

  geode->addDrawable( geometry.get() );

  return geode.release();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the XY Axis, and set at the Min Z
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initXYMin()
{
  osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
  osg::Vec3 myCoords[] =
  {
    osg::Vec3(_xMin, _yMin, _zMin),
    osg::Vec3(_xMin, _yMax, _zMin),
    osg::Vec3(_xMax, _yMax, _zMin),
    osg::Vec3(_xMax, _yMin, _zMin)
  };
  
  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);
  
  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.05f,0.05f,0.05f,0.25f));
  
    // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(0.0f,0.0f,-1.0f));
        // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);
  
        // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
  
        // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
  
        // This time we simply use primitive, and hardwire the number of coords to use 
        // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));
  
    // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();
  
  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);
  
  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the XY Axis, and set at the Max Z
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initXYMax()
{
    osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
    osg::Vec3 myCoords[] =
  {
    osg::Vec3(_xMax, _yMin, _zMax),
    osg::Vec3(_xMax, _yMax, _zMax),
    osg::Vec3(_xMin, _yMax, _zMax),
    osg::Vec3(_xMin, _yMin, _zMax)
  };

  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.05f,0.05f,0.05f,0.25f));

      // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(0.0f,0.0f,1.0f));
          // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);

          // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

          // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

          // This time we simply use primitive, and hardwire the number of coords to use 
          // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

      // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);

  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the XZ Axis, and set at the Min Y
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initXZMin()
{
  osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
  osg::Vec3 myCoords[] =
  {
    osg::Vec3(_xMax, _yMin, _zMax),
    osg::Vec3(_xMin, _yMin, _zMax),
    osg::Vec3(_xMin, _yMin, _zMin),
    osg::Vec3(_xMax, _yMin, _zMin)
  };

  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.20f,0.00f,0.00f,0.25f));

  // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
  // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);

  // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

  // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

  // This time we simply use primitive, and hardwire the number of coords to use 
  // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

  // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);

  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the XZ Axis, and set at the Max Y
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initXZMax()
{
    osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
    osg::Vec3 myCoords[] =
  {
    osg::Vec3(_xMax, _yMax, _zMin),
    osg::Vec3(_xMin, _yMax, _zMin),
    osg::Vec3(_xMin, _yMax, _zMax),
    osg::Vec3(_xMax, _yMax, _zMax)
  };

  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.20f,0.00f,0.00f,0.25f));

        // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
            // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);

            // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

            // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

            // This time we simply use primitive, and hardwire the number of coords to use 
            // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

        // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);

  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the YZ Axis, and set at the Min X
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initYZMin()
{
  osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
  osg::Vec3 myCoords[] =
  {
    osg::Vec3(_xMin, _yMax, _zMin),  
    osg::Vec3(_xMin, _yMin, _zMin),
    osg::Vec3(_xMin, _yMin, _zMax),
    osg::Vec3(_xMin, _yMax, _zMax)
  };

  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.0f,0.00f,0.20f,0.25f));

        // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(-1.0f,0.0f,0.0f));
            // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);

            // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

            // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

            // This time we simply use primitive, and hardwire the number of coords to use 
            // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

        // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);

  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Init the Plane that is parallel with the YZ Axis, and set at the Max X
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* GlassBoundingBox::_initYZMax()
{
  osg::ref_ptr < osg::Geometry > polyGeom ( new osg::Geometry );
  osg::Vec3 myCoords[] =
  {
  osg::Vec3(_xMax, _yMax, _zMax),
  osg::Vec3(_xMax, _yMin, _zMax),
  osg::Vec3(_xMax, _yMin, _zMin),
  osg::Vec3(_xMax, _yMax, _zMin)
  };

  int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

  osg::Vec3Array* vertices = new osg::Vec3Array(numCoords, myCoords);
  osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
  shared_colors->push_back(osg::Vec4(0.00f,0.00f,0.20f,0.25f));

  // same trick for shared normal.
  osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
  shared_normals->push_back(osg::Vec3(1.0f,0.0f,0.0f));
  // pass the created vertex array to the points geometry object.
  polyGeom->setVertexArray(vertices);

  // use the color array.
  polyGeom->setColorArray(shared_colors.get());
  polyGeom->setColorBinding(osg::Geometry::BIND_OVERALL);

  // use the normal array.
  polyGeom->setNormalArray(shared_normals.get());
  polyGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);

  // This time we simply use primitive, and hardwire the number of coords to use 
  // since we know up front,
  polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,numCoords));

  // add the points geomtry to the geode.
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::StateSet* stateset = polyGeom->getOrCreateStateSet();

  stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
  osg::AlphaFunc* alphaFunc = new osg::AlphaFunc;
  alphaFunc->setFunction(osg::AlphaFunc::GEQUAL,0.05f);
  stateset->setAttributeAndModes( alphaFunc,osg::StateAttribute::ON );
  stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
  stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  polyGeom->setStateSet(stateset);

  geode->addDrawable( polyGeom.get() );

  osg::ref_ptr< osg::StateSet > ss ( geode->getOrCreateStateSet() );
  ss->setMode(GL_CULL_FACE,osg::StateAttribute::ON );

  return geode.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for X
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::xMax ( const float x ) {  _xMax = x; }
float GlassBoundingBox::xMax ()  { return _xMax; }
void  GlassBoundingBox::xMin ( float x ) {  _xMin = x; }
float GlassBoundingBox::xMin ()  {  return _xMin; }

///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for Y
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::yMax ( float y ) {  _yMax = y; }
float GlassBoundingBox::yMax ()  { return _yMax; }
void  GlassBoundingBox::yMin ( float y ) {  _yMin = y; }
float GlassBoundingBox::yMin ()  {  return _yMin; }

///////////////////////////////////////////////////////////////////////////////
//
//  Get/Set the Maximum value for Z
//
///////////////////////////////////////////////////////////////////////////////

void  GlassBoundingBox::zMax ( float z ) {  _zMax = z; }
float GlassBoundingBox::zMax ()  { return _zMax; }
void  GlassBoundingBox::zMin ( float z ) {  _zMin = z; }
float GlassBoundingBox::zMin ()  {  return _zMin; }


///////////////////////////////////////////////////////////////////////////////
//
// Creates the text numbers at the corners of the bounding box
//
///////////////////////////////////////////////////////////////////////////////


osg::Node* GlassBoundingBox::_makeNumbers    ( )
{
  osg::BoundingBox bb( _xMin,_yMin,_zMin,_xMax,_yMax,_zMax );

  // Make the vertices
  osg::ref_ptr< osg::Vec3Array > vertices ( new osg::Vec3Array );
  
  // Fill the vertices
  for( unsigned int i = 0; i < 8; ++i ) 
    vertices->push_back ( bb.corner( i ) );

  osg::ref_ptr< osg::Geode >   geode ( new osg::Geode );

  osg::Vec4 layoutColor(1.0f,0.0f,0.0f,1.0f);
  float layoutCharacterSize = 0.055f * bb.radius();    
  
  char name[75];
  sprintf(name, "%0.0f,%0.0f,%0.0f", _xMin, _yMin, _zMin);
 
  geode->addDrawable( this->_makeNumber ( name,  bb.corner( 0 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  
  geode->addDrawable( this->_makeNumber ( _xMax, bb.corner( 1 ) * 1.05, layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( _yMax, bb.corner( 2 ) * 1.05, layoutColor, layoutCharacterSize  ) );
 // geode->addDrawable( this->_makeNumber ( "3", bb.corner( 3 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( _zMax, bb.corner( 4 ) * 1.05, layoutColor, layoutCharacterSize  ) );
 // geode->addDrawable( this->_makeNumber ( "5", bb.corner( 5 ), layoutColor, layoutCharacterSize  ) );
 // geode->addDrawable( this->_makeNumber ( "6", bb.corner( 6 ), layoutColor, layoutCharacterSize  ) );
  geode->addDrawable( this->_makeNumber ( "Max", bb.corner( 7 ) * 1.05, layoutColor, layoutCharacterSize  ) );

  return geode.release();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Creates the osg::Drawable that is the text to place at the bounding box corner
//
///////////////////////////////////////////////////////////////////////////////

osg::Drawable* GlassBoundingBox::_makeNumber     ( float number, const osg::Vec3& pos, const osg::Vec4& color, float size )
{
  char name[50];
  sprintf(name, "%0.3f", number);
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );
  //osgText::Font* arial = osgText::readFontFile( OsgTools::Font::fontfile( "fudd" ) );
  text->setFont( OsgTools::Font::defaultFont());
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( pos );
  text->setText( name );
  text->setAutoRotateToScreen(true);
  text->setFontResolution(40,40);
  text->setUseDisplayList(false);
  text->update();

  return text.release();
}

///////////////////////////////////////////////////////////////////////////////
//
// Creates the osg::Drawable that is the text. This version takes a string
//
///////////////////////////////////////////////////////////////////////////////

osg::Drawable* GlassBoundingBox::_makeNumber     ( const std::string& name, const osg::Vec3& pos, const osg::Vec4& color, float size )
{
  osg::ref_ptr< osgText::Text > text ( new osgText::Text );
//  osgText::Font* arial = osgText::readFontFile( OsgTools::Font::fontfile( "fudd" ) );
  text->setFont( OsgTools::Font::defaultFont() );
  text->setColor( color );
  text->setCharacterSize( size );
  text->setPosition ( pos );
  text->setText( name );
  text->setAutoRotateToScreen(true);
  text->setFontResolution(40,40);
  text->setUseDisplayList(false);
  text->update();

  return text.release();
}

