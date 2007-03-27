///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgTools/Sample.h"
#include "OsgTools/GraphStreamVisitor.h"
#include "osgProducer/Viewer"
#include "osg/Node"

int main(unsigned int argc,char* argv[])
{
  osg::ref_ptr<osgProducer::Viewer> viewer = new osgProducer::Viewer();
  viewer->setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

  OsgTools::Sample sample;
  osg::ref_ptr<osg::Node> scene = sample.scene();

  osg::ref_ptr<OsgTools::GraphStreamVisitor> streamer = new OsgTools::GraphStreamVisitor(std::cout);
  scene->accept(*streamer);

  viewer->setSceneData( scene.get() );
  viewer->realize();

  while( !viewer->done() )
  {
    viewer->sync();
    viewer->update();
    viewer->frame();
  }

  viewer->sync();

  return 1;
}
