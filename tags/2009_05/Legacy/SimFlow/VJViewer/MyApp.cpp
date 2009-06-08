
#include "MyApp.h"

#include <fstream>

#include "osg/Group"
#include "osg/Node"
#include "osg/PolygonMode"
#include "osg/LineSegment"

#include "vjCore/Types.h"

#include "gmtl/Matrix.h"
#include "gmtl/Math.h"   
#include "gmtl/MatrixOps.h"

#include "App/Parser.h"
#include "App/AppSAX2Handler.h"

MyApp::MyApp( vrj::Kernel* kern, int& argc, char** argv ) : OsgVJApp( kern, argc, argv )
{
	/// Initialize member variables here.
	//count = 0;
}

///////////////////////////////////////////////////////////////////////////////
//
// One time initialization. 
//
///////////////////////////////////////////////////////////////////////////////
void MyApp::appInit()
{
	setDevice(ALL, OFF);	
	setBackgroundColor((gmtl::Vec4f(0.30, 0.15, 0.55, 1.0)).getData());	
	setSceneInitialPosition(osg::Vec3(0.0, 0.0, 0.0));
	setNearFar(0.1, 1000000000.0);

	myEngine = new MyInteractionEngine();
	setEngine( myEngine );

	// Current interaction engine can be queried in appInit() now. 
	//myEngine = getEngine();	
	//myEngine->setTransSpeed( 10.0 );
	//myEngine->setTransDelta( 1.0 );

	// App has to initialize and update KeyboradMouseHandler object. 
	//mKMHandle.init();	
} 

///////////////////////////////////////////////////////////////////////////////
//
// Clear OpenGL buffers. 
//
///////////////////////////////////////////////////////////////////////////////
void MyApp::appBufferPreDraw()
{
	// Set background color to black.
	glClearColor(0.5, 0.5, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
}

///////////////////////////////////////////////////////////////////////////////
//
// Scene initialization. 
//
///////////////////////////////////////////////////////////////////////////////
void MyApp::appSceneInit()
{	
  const std::string sceneFile( "scene.sfd" );
  
  // Create a scene manager. 
  sm = &( SimFlow::Scene::SceneManager::instance() );

  // App specific handler for XML. 
  SimFlow::App::AppSAX2Handler* handler = new SimFlow::App::AppSAX2Handler();
  handler->sceneManager( sm.get() );
	
  // Parser object. 
  SimFlow::App::Parser* parser = new SimFlow::App::Parser();
  parser->init();
  parser->setHandler( handler );
  parser->readXML( sceneFile );

  sm->buildScene();

  mModelGroupNode->addChild( sm->root() );
}

void MyApp::appPreFrame()
{
	if( myEngine->getButton1Status() )
	{
		std::cout << "Toggle animation: " << std::endl;
		sm->animate( !sm->animate() );
		myEngine->setButton1Pressed( false );
	}
}

void MyApp::appLatePreFrame()
{
  sm->update();
}

