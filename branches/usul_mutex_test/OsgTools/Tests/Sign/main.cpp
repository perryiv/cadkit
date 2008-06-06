
#include <osgProducer/Viewer>

#include "OsgTools/Builders/Sign.h"

int main ( int argc, char** argv )
{
  // initialize the viewer.
  osgProducer::Viewer viewer;

  // set up the value with sensible default event handlers.
  viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
  

  OsgTools::Builders::Sign sign;

  // set the scene to render
  viewer.setSceneData( sign() );

  // create the windows and run the threads.
  viewer.realize();

  while( !viewer.done() )
  {
    // wait for all cull and draw threads to complete.
    viewer.sync();

    // update the scene by traversing it with the the update visitor which will
    // call all node update callbacks and animations.
    viewer.update();
     
    // fire off the cull and draw traversals of the scene.
    viewer.frame();
  }
  
  // wait for all cull and draw threads to complete.
  viewer.sync();

  // run a clean up frame to delete all OpenGL objects.
  viewer.cleanup_frame();

  // wait for all the clean up frame to complete.
  viewer.sync();

  return 0;
}
