/*
 *  GlassBoundingBox.cpp
 *  OsgTools
 *
 *  Created by Michael A Jackson on 7/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#include "OsgTools/GlassBoundingBox.h"
#include <algorithm>
#include <functional>

#include "osg/Group"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec4"
#include "osg/LineWidth"
#include "osg/StateSet"
#include "osg/AlphaFunc"
#include "osg/StateSet"

#include <limits>

using namespace OsgTools;


USUL_IMPLEMENT_TYPE_ID ( GlassBoundingBox );


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




void GlassBoundingBox::addBoundingGlass(osg::Group *root) 
{
  root->addChild(this->_initXYMin() );
  root->addChild(this->_initXYMax() );
  root->addChild(this->_initXZMax() );
  root->addChild(this->_initXZMin() );
  root->addChild(this->_initYZMax() );
  root->addChild(this->_initYZMin() );
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




