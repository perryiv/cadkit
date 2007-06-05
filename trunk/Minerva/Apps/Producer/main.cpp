
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
//  Set for multi-threaded. This is global because the sooner we set this, 
//  the better. Setting in main() may be too late.
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Threads/Mutex.h"
#include "Threads/OpenThreads/Mutex.h"

Usul::Threads::SetMutexFactory factory ( &Threads::OT::newOpenThreadsMutex );

#include "DrawCallbackImpl.h"

#include "Minerva/Core/DB/Connection.h"
#include "Minerva/Core/Viz/Controller.h"
#include "Minerva/Core/Scene/SceneManager.h"
#include "Minerva/Core/DataObjects/UserData.h"

#include "Magrathea/Planet.h"

#include "Usul/App/Controller.h"

#include "Usul/CommandLine/Arguments.h"

#include "Usul/Policies/Update.h"

#include "osgProducer/Viewer"

#include "osgDB/WriteFile"
#include "osgDB/Registry"

#ifdef _MSC_VER
# include <direct.h>
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Main function.
//
///////////////////////////////////////////////////////////////////////////////

int main ( int argc, char **argv )
{
  // Set command-line arguments.
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

  std::string session ( "" );

  if ( argc == 2 )
    session = argv[1];
  else
  {
    std::cout << "Warning: No session specified, using default.";
    session = "default";
  }

  // use an ArgumentParser object to manage the program arguments.
  osg::ArgumentParser arguments(&argc,argv);
  
  // construct the viewer.
  osgProducer::Viewer viewer(arguments);

  // set up the value with sensible default event handlers.
  viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

  // create the windows and run the threads.
  viewer.realize();

  Minerva::Core::DB::Connection::RefPtr applicationConnection ( new Minerva::Core::DB::Connection );
  applicationConnection->username( "wnv_app" );
  applicationConnection->password( "wnv" );
  applicationConnection->database( "wnv_application" );
  applicationConnection->hostname ( "cinema.dt.asu.edu" );
  //applicationConnection->hostname ( "localhost" );
  applicationConnection->connect();

  // Create the database manager.
  Minerva::Core::Viz::Controller::RefPtr dbManager ( new Minerva::Core::Viz::Controller );
  dbManager->applicationConnection( applicationConnection.get() );
  dbManager->connectToSession( session );

  DrawCallbackImpl::RefPtr callback ( new DrawCallbackImpl ( viewer ) );

  dbManager->callback ( callback.get() );

  // Make the scene manager.
  Minerva::Core::Scene::SceneManager::RefPtr manager ( new Minerva::Core::Scene::SceneManager );
  dbManager->sceneManager ( manager.get() );

  // Set the scene.
  viewer.setClearColor( osg::Vec4( 0.7f, 0.7f, 0.7f, 1.0f ) );

  Usul::Policies::TimeBased update ( 1000 );

  osg::ref_ptr < Magrathea::Planet > planet ( new Magrathea::Planet );
  planet->init();
  planet->root()->addChild ( manager->root() );

#ifdef _MSC_VER
  ::chdir ( "F:\\data\\images\\earth" );
  planet->readKWL( "F:\\data\\images\\earth\\blue_marble.kwl" );
#endif

  osgDB::Registry::instance()->setDatabasePager( planet->databasePager() );

  unsigned int num = viewer.addCameraManipulator( planet->manipulator() );
  viewer.selectCameraManipulator(num);

  viewer.setSceneData( planet->root() );

  // Enter the render loop.
  while( !viewer.done() )
  {
    // wait for all cull and draw threads to complete.
    viewer.sync();

    GLint viewport[4];

    ::glGetIntegerv(GL_VIEWPORT, viewport);

    manager->resize ( static_cast < unsigned int > ( viewport[2] ), static_cast < unsigned int > ( viewport[3] ) );

    // If there are draw commands to process...
    if( update() && dbManager->hasEvents() )
    {
      // Update the scene.
      dbManager->updateScene();
      manager->buildScene();
    }

    // update the scene by traversing it with the the update visitor which will
    // call all node update callbacks and animations.
    viewer.update();
     
    // fire off the cull and draw traversals of the scene.
    viewer.frame();
  }

  planet = 0x0;

  // Clear any internal data left.
  manager->clear();
  
  // wait for all cull and draw threads to complete before exit.
  viewer.sync();

  // run a clean up frame to delete all OpenGL objects.
  viewer.cleanup_frame();

  // wait for all the clean up frame to complete.
  viewer.sync();

  return 0;
}
