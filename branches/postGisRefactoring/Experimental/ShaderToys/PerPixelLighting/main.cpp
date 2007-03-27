
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "osgProducer/Viewer"

#include "BuildScene.h"

int main ( int argc, char **argv )
{
  osg::ArgumentParser arguments(&argc,argv);
  
  // construct the viewer.
  osgProducer::Viewer viewer(arguments);

  // set up the value with sensible default event handlers.
  viewer.setUpViewer( osgProducer::Viewer::STANDARD_SETTINGS );

  // create the windows and run the threads.
  viewer.realize();

  osg::ref_ptr < osg::Node > node ( buildScene() );

  // Set the scene.
  viewer.setSceneData( node.get() );

  osg::ref_ptr< osg::StateSet > ss ( node->getOrCreateStateSet() );

  // Enter the render loop.
  while( !viewer.done() )
  {
    //osg::Uniform* uniform = ss->getOrCreateUniform("osg_ViewMatrix",osg::Uniform::FLOAT_MAT4);
    //uniform->set( viewer.getViewMatrix() );

    // wait for all cull and draw threads to complete.
    viewer.sync();

    // update the scene by traversing it with the the update visitor which will
    // call all node update callbacks and animations.
    viewer.update();
     
    // fire off the cull and draw traversals of the scene.
    viewer.frame();
  }
  
  // wait for all cull and draw threads to complete before exit.
  viewer.sync();

  // run a clean up frame to delete all OpenGL objects.
  viewer.cleanup_frame();

  // wait for all the clean up frame to complete.
  viewer.sync();


  return 0;
}
