
#ifndef _MY_APP_H_
#define _MY_APP_H_

#include "vjCore/OsgVJApp.h"

#include "Scene/SceneManager.h"

#include "MyInteractionEngine.h"

class MyApp : public OsgVJApp 
{
	public:

		MyApp( vrj::Kernel* kern, int& argc, char** argv );
		~MyApp(){;}		

		void appInit();		
		void appBufferPreDraw();	
	  void appSceneInit();	
		void appPreFrame();
		void appLatePreFrame();		

	private:			
		MyInteractionEngine* myEngine;

    osg::ref_ptr< SimFlow::Scene::SceneManager > sm;
};

#endif // _MY_APP_H_

