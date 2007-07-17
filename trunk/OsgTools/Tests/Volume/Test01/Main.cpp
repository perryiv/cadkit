
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  This is a test of the proxy geometry for the 3D texture.
//
///////////////////////////////////////////////////////////////////////////////

#include "osgViewer/Viewer"

#include "OsgTools/Box.h"
#include "OsgTools/State/StateSet.h"
#include "OsgTools/Volume/PlanarProxyGeometry.h"

#include "osg/Geode"
#include "osg/Group"
#include "osg/MatrixTransform"


///////////////////////////////////////////////////////////////////////////////
//
//  Build the bounding box to draw.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildBoundingBox( const osg::BoundingBox& bb )
{
  OsgTools::ColorBox box ( bb );
  box.color_policy().color ( osg::Vec4 ( 1, 1, 1, 1 ) );

  // Position it.
  osg::ref_ptr<osg::MatrixTransform> mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( bb.center() ) );
  mt->addChild ( box() );

  // Wire-frame.
  OsgTools::State::StateSet::setPolygonsLines ( mt.get(), true );
  OsgTools::State::StateSet::setLighting ( mt.get(), false );
	
	return mt.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )
{
  osgViewer::Viewer viewer;

  viewer.setUpViewInWindow( 0, 500, 512, 512 );

  typedef OsgTools::Volume::PlanarProxyGeometry Drawable;

  osg::ref_ptr < osg::Group > root ( new osg::Group );

  osg::BoundingBox bb ( -1.0, -1.0, -1.0, 1.0, 1.0, 1.0 );

  root->addChild ( buildBoundingBox( bb ) );

  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < Drawable > drawable ( new Drawable );
  drawable->numPlanes ( 1024 );
  drawable->boundingBox ( bb );

  OsgTools::State::StateSet::setPolygonsLines ( geode.get(), true );
  OsgTools::State::StateSet::setLighting      ( drawable->getOrCreateStateSet(), false );
  
  geode->addDrawable ( drawable.get() );

  root->addChild ( geode.get( ) );

  viewer.setSceneData ( root.get() );

  viewer.realize();

	return viewer.run();
}
