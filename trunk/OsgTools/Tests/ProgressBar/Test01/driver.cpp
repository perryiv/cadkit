
#include "Usul/CommandLine/Arguments.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "OsgTools/Widgets/ThreadSafeProgressBarGroup.h"

#include <osg/Geode>
#include <osg/Node>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>

#include <osg/Math>

class ThreadSafeProgressBarGroupCallback : public osg::NodeCallback
{
  public:
  //typedef osg::Drawable::UpdateCallback   BaseClass;

  ThreadSafeProgressBarGroupCallback::ThreadSafeProgressBarGroupCallback ( OsgTools::Widgets::ThreadSafeProgressBarGroup * pbarGroup ) : 
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
  virtual ThreadSafeProgressBarGroupCallback::~ThreadSafeProgressBarGroupCallback()
  {
    _pbarGroup = 0x0;
  }
private:
 
  OsgTools::Widgets::ThreadSafeProgressBarGroup * _pbarGroup;
};

class DriverEventHandler : public osgGA::GUIEventHandler
{
public:
    
  DriverEventHandler( OsgTools::Widgets::ThreadSafeProgressBarGroup* b ):
      _bar( b ), _speed ( 3.5f ), _barToChange ( 0 )
             {}
    
        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
        {
            switch(ea.getEventType())
            {
              case(osgGA::GUIEventAdapter::KEYDOWN):
		          {
                if ( ea.getKey() == osgGA::GUIEventAdapter::KEY_Home )
		            {
			           /* if(_bar->isVisible( _barToChange ) )
			            {
				            _bar->hideProgressBar( _barToChange );
				            
			            }
			            else
			            {
				            _bar->showProgressBar( _barToChange );
				            
			            }*/

		            }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Insert )
                {
                  _speed += 0.5f;
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Delete )
                {
                  _speed -= 0.5f;
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_End )
                {
                 /* if( _bar->isRelativeToAbsolute() )
                    _bar->setRelativeToAbsolute( false );
                  else
                    _bar->setRelativeToAbsolute( true );*/
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Down  )
                {
                  //_bar->add ( "Additional Bar" , 0, 100 );
                  _bar->append();
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Up  )
                {
                 /* if(_bar->getNumItems() > 0)
                    _bar->remove ( _barToChange );

                  if( _barToChange > 0 )
                     _barToChange -= 1;*/
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Left )
                {
                  if( _bar->getNumBars() > 0 )
                    _bar->setBarValue( _barToChange, _bar->getBarValue( _barToChange ) - _speed );
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Right )
                {
                  if( _bar->getNumBars() > 0 )
                   _bar->setBarValue( _barToChange, _bar->getBarValue( _barToChange ) + _speed );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Up  )
                {
                  if( _barToChange < _bar->getNumBars() - 1 )
                    ++_barToChange;
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Down )
                {
                  if( _barToChange > 0 )
                    --_barToChange;
                }
                
                return false;
              }
              default:
                return false;
            }
        }
        
		osg::ref_ptr< OsgTools::Widgets::ThreadSafeProgressBarGroup > _bar;
    float _speed;
    int _barToChange;
        
};


int main(int argc , char ** argv)
{
	Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex ); 

  Usul::CommandLine::Arguments::instance().set ( argc, argv );
	
	osg::ref_ptr< osg::Group > render_group ( new osg::Group() );

  OsgTools::Widgets::ThreadSafeProgressBarGroup::RefPtr pbar ( new OsgTools::Widgets::ThreadSafeProgressBarGroup() );
  //pbar->setLocation ( OsgTools::Widgets::ProgressBarGroup::LOWER_RIGHT );
  pbar->append ();
  //OsgTools::Widgets::ThreadSafeProgressBar::RefPtr bar ( new OsgTools::Widgets::ThreadSafeProgressBar ( 0 ) );

  render_group->setUpdateCallback ( new ThreadSafeProgressBarGroupCallback ( pbar.get() ) );
  pbar->buildScene();
  //osg::ref_ptr<osg::Node> node = pbar->getScene();
  //node->setUpdateCallback( new ThreadSafeProgressBarGroupCallback ( pbar.get() ) );
  render_group->addChild( pbar->getScene() );

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
