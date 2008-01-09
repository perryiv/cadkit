
#include "MyApp.h"

#include <fstream>

#include "osg/ImageStream"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/Material"
#include "osg/Texture2D"
#include "osg/TextureRectangle"
#include "osg/TexMat"
#include "osg/CullFace"

#include "Core/Movie.h"

MyApp::MyApp( vrj::Kernel* kern, int& argc, char** argv ) : OsgVJApp( kern, argc, argv )
{
}

void MyApp::appInit()
{
	setDevice(ALL, OFF);	
	setBackgroundColor((gmtl::Vec4f(0.35, 0.35, 0.35, 1.0)).getData());	
}
void MyApp::appBufferPreDraw()
{
	glClearColor(0.5, 0.5, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void MyApp::appSceneInit()
{	
  std::cerr << "Scene initialization starting: " << std::endl;

	osg::Vec3 pos(0.0f,0.0f,0.0f);

  #ifdef _WIN32
    osgDB::DynamicLibrary::loadLibrary("osgdb_xine.dll"); 
  #elif
    osgDB::DynamicLibrary::loadLibrary("osgdb_xine.so");        
  #endif 
  
  movie =  Cinema::Core::Movie::instance();	
  movie->setMovie( pos, "test.avi" );
	movie->buildScene();	
		
	mModelGroupNode->addChild( movie->root() );	

  std::cerr << "Scene initialization ending: " << std::endl;
}

void  MyApp::appLatePreFrame()
{
  std::cout << "Frame number is: " << mFrameNumber << std::endl;
  if( mFrameNumber == 100 ) 
  {
    movie->restart();
  }
}