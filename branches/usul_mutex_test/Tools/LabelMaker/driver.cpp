#include "LabelReader.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Manager.h"
#include "Usul/Policies/Update.h"
#include "Usul/File/Find.h"
#include "Usul/Components/Manager.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>
//#include <osgViewer/StatsHandler> 
#include <osgViewer/ViewerEventHandlers> 
#include <osgGA/StateSetManipulator> 

#include <osgDB/WriteFile>

#include <osg/Math>

#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iostream>

class DriverEventHandler : public osgGA::GUIEventHandler
{
public:
    
  DriverEventHandler( osgViewer::Viewer* viewer, LabelReader * labels ) :
             _viewer ( viewer ),
             _labels ( labels ),
             _buildBox ( false )
             {}
    
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
        {
            switch(ea.getEventType())
            {
              case(osgGA::GUIEventAdapter::KEYDOWN):
		          {
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Insert )
                {
                  std::cout << "Building scene with bounding box" << std::endl;
                  _labels->buildScene( true );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete )
                {
                  
                  std::cout << "Building scene without bounding box" << std::endl;
                  _labels->buildScene( false );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Home )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_End )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Down  )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Up  )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Left )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Right )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Return )
                {
                  // Feedback
                  std::cout << "Writing file: output.osg" << std::endl;
                  osgDB::writeNodeFile(*(_viewer->getSceneData()), "output.osg" );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Up  )
                {
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Down )
                {
                 
                }
                
                return false;
              }
              default:
                return false;
            }
        }
        ~DriverEventHandler()
        {
          
        }
private:
      osg::ref_ptr< osgViewer::Viewer > _viewer;
      LabelReader::RefPtr _labels;
      bool _buildBox;
            
};

struct Parameters
{
  bool writeSingleFrame;
  bool takeMultipleFrames;
};
int main(int argc , char ** argv)
{
  Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );
  Usul::Threads::Manager::instance().factory ( &Threads::OT::newOpenThreadsThread );
  Usul::CommandLine::Arguments::instance().set ( argc, argv );

	osg::ref_ptr< osg::Group > render_group ( new osg::Group() );
  LabelReader::RefPtr labels ( new LabelReader );
  labels->read( "data/input.txt" );

  int result ( 1 );
  {
    // construct the viewer.
    osg::ref_ptr< osgViewer::Viewer > viewer ( new osgViewer::Viewer() );
   
   
    render_group->addChild( labels->buildScene( true ) );
	  viewer->setSceneData( render_group.get() );
 
    viewer->addEventHandler( new DriverEventHandler( viewer.get(), labels.get() ) );
    viewer->addEventHandler( new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet())); 
    viewer->addEventHandler( new osgViewer::StatsHandler()); 
    viewer->addEventHandler( new osgViewer::WindowSizeHandler()); 

    result = viewer->run();
  }

  Usul::Jobs::Manager::instance().wait();
  Usul::Components::Manager::instance().clear ( &std::cout );
  Usul::Jobs::Manager::destroy();
  Usul::Threads::Manager::instance().purge();
  Usul::Threads::Mutex::createFunction ( 0x0 );
  Usul::Threads::Manager::instance().factory ( 0x0 );

  return result;	
	
	
}
