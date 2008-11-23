
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_TESTS_RUN_OSG_VIEWER_TEST_H__
#define __CADKIT_TESTS_RUN_OSG_VIEWER_TEST_H__

#include "Usul/Factory/ObjectFactory.h"
#include "Usul/Registry/Database.h"

#include "osgViewer/Viewer"
#include "osgGA/StateSetManipulator"

namespace CadKit {
namespace Tests {
namespace OSG {


inline int runTestNoUsulThreads ( osg::Node* scene )
{
  // Make the viewer.
  osgViewer::Viewer viewer;

  // This allows 'w' -> wireframe.
  viewer.addEventHandler( new osgGA::StateSetManipulator( viewer.getCamera()->getOrCreateStateSet() ) );

  // Add the scene to the viewer.
  viewer.setSceneData ( scene );

  // Run the viewer.
  const int result ( viewer.run() );

  // Clear the registry.
  Usul::Registry::Database::destroy();

  // Clear the ObjectFactory.
  Usul::Factory::ObjectFactory::instance().clear();

  // return the result.
  return result;
}


}
}
}

#endif // __CADKIT_TESTS_RUN_OSG_VIEWER_TEST_H__
