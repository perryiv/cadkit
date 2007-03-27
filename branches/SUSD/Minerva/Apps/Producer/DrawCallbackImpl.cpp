
#include "DrawCallbackImpl.h"

#include <osgProducer/Viewer>



DrawCallbackImpl::DrawCallbackImpl( osgProducer::Viewer& viewer ) : _viewer ( viewer )
{
}

DrawCallbackImpl::~DrawCallbackImpl()
{
}


void DrawCallbackImpl::draw()
{
  // wait for all cull and draw threads to complete.
  _viewer.sync();

  // update the scene by traversing it with the the update visitor which will
  // call all node update callbacks and animations.
  _viewer.update();
   
  // fire off the cull and draw traversals of the scene.
  _viewer.frame();
}

