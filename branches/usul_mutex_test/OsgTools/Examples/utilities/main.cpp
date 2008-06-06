///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

/** Utilities Demo
  * This file uses some analysis classes included within OsgTools.
  * The classes include Dimension, a class to determine the size of one drawable,
  * _____, a class that uses Dimension to determine the size of all drawables,
  * and finally, _____, a class that uses both ______ and ______ to construct
  * a _____ and attach the geode to tree.
  */

#include "OsgTools/GraphStreamVisitor.h"
#include "OsgTools/Sample.h"
#include <osgProducer/Viewer>
#include "Display.h"

int main(unsigned int argc, char* argv[])
{
  // make a sample scene
  OsgTools::Sample sample;
  osg::ref_ptr<osg::Node> scene( sample.scene() );

  // print out the tree to make sure it is good
  osg::ref_ptr<OsgTools::GraphStreamVisitor> gsv = new OsgTools::GraphStreamVisitor(std::cout);
  scene->accept(*gsv);

  // make a viewer, set it up, add the data
  osgProducer::Viewer viewer;
  viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);
  viewer.setSceneData( scene.get() );

  // make an eventhandler and add it to the viewer
  Display controller(viewer);

  // start the viewing process
  viewer.realize();

  while( !viewer.done() )
  {
    viewer.sync();
    viewer.update();
    viewer.frame();
  }

  viewer.sync();

  return 1;
}
