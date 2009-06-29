
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "FastLoadTriangleSet.h"

#include "Usul/Adaptors/Random.h"

#include "osg/Group"
#include "osg/Geometry"
#include "osg/Material"


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

FastLoadTriangleSet::FastLoadTriangleSet() :
_bb(),
_groups()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

FastLoadTriangleSet::~FastLoadTriangleSet()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a group.
//
///////////////////////////////////////////////////////////////////////////////

void FastLoadTriangleSet::addGroup ( osg::Vec3Array *vertices, osg::Vec3Array *normalsT, osg::Vec3Array *normalsV, osg::DrawElementsUInt *indices )
{
  _groups.push_back ( new Group ( vertices, normalsT, normalsV, indices ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the number of groups.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int FastLoadTriangleSet::numberOfGroups () const
{
  return _groups.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the primitive group at the given index.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* FastLoadTriangleSet::getPrimitiveGroup ( unsigned int i )
{
  //return _groups.at( i )->queryInterface ( Usul::Interfaces::ISceneElement::IID );
	return 0x0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

osg::BoundingBox FastLoadTriangleSet::boundingBox() const
{
  return _bb;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* FastLoadTriangleSet::buildScene ( const Options &opt, Unknown *caller )
{
  osg::ref_ptr < osg::Group > root ( new osg::Group );

  for ( unsigned int i = 0; i < _groups.size(); ++i )
  {
    root->addChild ( _groups.at( i )->buildScene( opt, caller ) );
  }

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the triangle set empty?
//
///////////////////////////////////////////////////////////////////////////////

bool FastLoadTriangleSet::empty() const
{
  return _groups.empty();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the bounding box.
//
///////////////////////////////////////////////////////////////////////////////

void FastLoadTriangleSet::updateBounds ( const osg::Vec3& vertex )
{
  _bb.expandBy ( vertex );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Decimate the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void FastLoadTriangleSet::decimate ( Usul::Interfaces::IDecimateTriangles* decimate, float reduction )
{
  for ( Groups::iterator iter = _groups.begin(); iter != _groups.end(); ++iter )
  {
    (*iter)->decimate( decimate, reduction );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Smooth the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void FastLoadTriangleSet::smooth ( Usul::Interfaces::ISmoothTriangles *smooth, unsigned int numIterations )
{
  for ( Groups::iterator iter = _groups.begin(); iter != _groups.end(); ++iter )
  {
    (*iter)->smooth( smooth, numIterations );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Subdivide the triangle set.
//
///////////////////////////////////////////////////////////////////////////////

void FastLoadTriangleSet::subdivide ( Usul::Interfaces::ISubdivideTriangles *subdivide, unsigned int numIterations )
{
  for ( Groups::iterator iter = _groups.begin(); iter != _groups.end(); ++iter )
  {
    (*iter)->subdivide( subdivide, numIterations );
  }
}