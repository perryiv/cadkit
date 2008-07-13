
#include "Usul/CommandLine/Arguments.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "OsgTools/Widgets/ProgressBar.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>

#include <osg/Math>

class ProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  //typedef osg::Drawable::UpdateCallback   BaseClass;

  ProgressBarGroupCallback ( OsgTools::Widgets::ProgressBar * pbarGroup ) : 
  _pbarGroup ( pbarGroup )
  {
  }
  
  virtual void operator() (osg::Node* node, osg::NodeVisitor* nv)
  {
    if( 0x0 != _pbarGroup )
    {
      _pbarGroup->buildScene(); 
    }
    traverse(node,nv);
  }

protected:
  virtual ~ProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ProgressBar * _pbarGroup;
};

class DriverEventHandler : public osgGA::GUIEventHandler
{
public:
    
  DriverEventHandler( OsgTools::Widgets::ProgressBar* b ):
      _bar( b ), _speed ( 3.5f ), _barToChange ( 0 )
             {}
    
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
        {
            switch(ea.getEventType())
            {
              case(osgGA::GUIEventAdapter::KEYDOWN):
		          {
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Insert )
                {
                  _speed += 0.5f;
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete )
                {
                  _speed -= 0.5f;
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Left )
                {
                   _bar->value( _bar->value( ) - _speed );
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Right )
                {
                   _bar->value( _bar->value( ) + _speed );
                }
                
                return false;
              }
              default:
                return false;
            }
        }
        
		osg::ref_ptr< OsgTools::Widgets::ProgressBar > _bar;
    float _speed;
    int _barToChange;
        
};


int main(int argc , char ** argv)
{
	Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex ); 

  Usul::CommandLine::Arguments::instance().set ( argc, argv );
	
	osg::ref_ptr< osg::Group > render_group ( new osg::Group() );

  OsgTools::Widgets::ProgressBar::RefPtr pbar ( new OsgTools::Widgets::ProgressBar() );
  pbar->setLowerLeft ( Usul::Math::Vec2f ( 0.0, 0.0 ) );

  render_group->setUpdateCallback ( new ProgressBarGroupCallback ( pbar.get() ) );
  pbar->buildScene();

  render_group->addChild( pbar->buildScene() );

  int result ( 1 );
  {
    // construct the viewer.
    osg::ref_ptr< osgViewer::Viewer > viewer ( new osgViewer::Viewer() );
    viewer->addEventHandler( new DriverEventHandler( pbar.get() ) );
	  viewer->setSceneData( render_group.get() );
    result = viewer->run();
  }

  return result;	
}
