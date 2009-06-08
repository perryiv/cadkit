
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
//  This is a test of cylinder creation in the shape factory.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/ShapeFactory.h"
#include "OsgTools/MaterialFactory.h"
#include "OsgTools/Callbacks/SortBackToFront.h"

#include "osg/Geode"
#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgGA/StateSetManipulator"

#include "osgViewer/Viewer"


///////////////////////////////////////////////////////////////////////////////
//
//  Build a cylinder.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildCylinder ( OsgTools::ShapeFactory* sf, const osg::Vec3& position, const osg::Vec3& v0, const osg::Vec3& v1, osg::Material* material, float alpha )
{
  bool transparent ( alpha < 1.0f );

  osg::ref_ptr < osg::MatrixTransform > mt ( new osg::MatrixTransform );
  mt->setMatrix ( osg::Matrix::translate ( position ) );
  
  osg::ref_ptr < osg::Geode > geode ( new osg::Geode );
  osg::ref_ptr < osg::Geometry > geometry ( sf->cylinder( 5.0, 20, v0, v1, !transparent ) );
  geode->addDrawable ( geometry.get() );

  mt->addChild ( geode.get() );

  osg::ref_ptr < osg::StateSet > ss ( mt->getOrCreateStateSet() );

  if ( transparent )
  {
    osg::ref_ptr < osg::Material > mat ( static_cast < osg::Material* > ( material->clone ( osg::CopyOp::DEEP_COPY_ALL ) ) );
    mat->setTransparency ( osg::Material::FRONT_AND_BACK, alpha );

    ss->setAttributeAndModes ( mat.get() );
    ss->setRenderingHint ( osg::StateSet::TRANSPARENT_BIN );

    ss->setMode( GL_BLEND, osg::StateAttribute::ON );

    geode->setCullCallback ( new OsgTools::Callbacks::SortBackToFront );
    geometry->setUseDisplayList ( false );
  }
  else
    ss->setAttributeAndModes ( material );

  return mt.release ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node* buildScene ()
{
  osg::ref_ptr < osg::Group > root ( new osg::Group );

  osg::ref_ptr < OsgTools::ShapeFactory > sf ( new OsgTools::ShapeFactory );
  osg::ref_ptr < OsgTools::MaterialFactory > mf ( new OsgTools::MaterialFactory );

  osg::Vec3 v0 ( 0.0, 0.0, 0.0 );
  osg::Vec3 v1 ( 0.0, -1.0, 0.0 );
  osg::Vec3 position ( 0.0 , 0.0, 0.0 );

  for ( unsigned int i = 0; i < 10; ++i )
  {
    osg::ref_ptr < osg::Material > material ( mf->create() );

    // The opaque cylinder
    {
      osg::ref_ptr < osg::Node > node ( buildCylinder ( sf.get(), position, v0, v1, material.get(), 0.85f ) );
      root->addChild ( node.get () );
    }

    position.x() += 15;

    // The transparent cylinder.
    {
      osg::ref_ptr < osg::Node > node ( buildCylinder ( sf.get(), position, v0, v1, material.get(), 0.85f ) );
      root->addChild ( node.get () );
    }

    position.x() += 15;

    // Make each cylinder taller than the last.
    v1.y() -= ( i + 1 );
  }

  return root.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Main.
//
///////////////////////////////////////////////////////////////////////////////

int main( int argc, char **argv )
{
  osgViewer::Viewer viewer;

  // add the state manipulator
  viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );

  viewer.setUpViewInWindow( 0, 500, 512, 512 );

  osg::ref_ptr < osg::Group > root ( new osg::Group );

  root->addChild ( buildScene( ) );

  viewer.setSceneData ( root.get() );

  viewer.realize();

	return viewer.run();
}
