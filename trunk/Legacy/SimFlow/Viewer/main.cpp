
#include "SimFlow/Core/Scene/SceneManager.h"
#include "SimFlow/Core/App/Parser.h"
#include "SimFlow/Core/App/AppSAX2Handler.h"

#include "osgGA/TrackballManipulator"

#include "osgViewer/Viewer"


class EventHandler : public osgGA::GUIEventHandler
{
public:
    
  EventHandler(  ) {}
    
  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
  {
    switch(ea.getEventType())
    {
      case(osgGA::GUIEventAdapter::KEYDOWN):
      {
        if ( ea.getKey() == 'p' )
          SimFlow::Scene::SceneManager::instance().animate( !SimFlow::Scene::SceneManager::instance().animate() );
      }
    }

    return false;
  }
};


int main( int argc, char **argv )
{
  // Create the viewer.
  osgViewer::Viewer viewer;

  // Create a scene manager. 
  SimFlow::Scene::SceneManager& sm = SimFlow::Scene::SceneManager::instance();

  // App specific handler for XML. 
  SimFlow::App::AppSAX2Handler* handler = new SimFlow::App::AppSAX2Handler();
  handler->sceneManager( &sm );
	
  // Parser object. 
  SimFlow::App::Parser* parser = new SimFlow::App::Parser();
  parser->init();
  parser->setHandler( handler );

  // Now parse the file. 
  if( argc < 2 )
  {
	  parser->readXML( "scene.sfd" );
  }
  else
  {
	  parser->readXML( std::string( argv[1] ) );
  }

  sm.buildScene();

  viewer.setSceneData( sm.root() );
  
  viewer.addEventHandler( new EventHandler );

  viewer.setCameraManipulator( new osgGA::TrackballManipulator );

  viewer.init();
  viewer.realize();

  while ( false == viewer.done() )
  {
    sm.update();

    viewer.frame();
  }

  return 0;
}
