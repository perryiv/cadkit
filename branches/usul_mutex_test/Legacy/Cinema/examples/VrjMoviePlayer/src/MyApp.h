
#ifndef __MY_APP_H__
#define __MY_APP_H__

#include "VrjCore/OsgVJApp.h"

#include "Core/Movie.h"

class MyApp : public VrjCore::OsgVJApp 
{
	public:

		MyApp( vrj::Kernel* kern, int& argc, char** argv );

    ~MyApp(){;}		

		void appInit();

    void appBufferPreDraw();		

    void appSceneInit();

    void appLatePreFrame();

	private:	

		osg::ref_ptr< Cinema::Core::Movie > movie;
};

#endif //: __MY_APP_H__

