
#include "Usul/CommandLine/Arguments.h"

#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include "OsgTools/Widgets/ProgressBarGroup.h"

#include <osg/Geode>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>

#include <osg/Math>


class DriverEventHandler : public osgGA::GUIEventHandler
{
public:
    
  DriverEventHandler( OsgTools::Widgets::ProgressBarGroup* b ):
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
			            if(_bar->isVisible( _barToChange ) )
			            {
				            _bar->hideProgressBar( _barToChange );
				            
			            }
			            else
			            {
				            _bar->showProgressBar( _barToChange );
				            
			            }

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
                  if( _bar->isRelativeToAbsolute() )
                    _bar->setRelativeToAbsolute( false );
                  else
                    _bar->setRelativeToAbsolute( true );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Down  )
                {
                  _bar->add ( "Additional Bar" , 0, 100 );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Page_Up  )
                {
                  if(_bar->getNumItems() > 0)
                    _bar->remove ( _barToChange );

                  if( _barToChange > 0 )
                     _barToChange -= 1;
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Left )
                {
                  if( _bar->getNumItems() > 0 )
                    _bar->setItemValue( _barToChange, _bar->getItemValue( _barToChange ) - _speed );
                  
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Right )
                {
                  if( _bar->getNumItems() > 0 )
                   _bar->setItemValue( _barToChange, _bar->getItemValue( _barToChange ) + _speed );
                }
                if( ea.getKey() == osgGA::GUIEventAdapter::KEY_Up  )
                {
                  if( _barToChange < _bar->getNumItems () - 1 )
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
        
		osg::ref_ptr< OsgTools::Widgets::ProgressBarGroup > _bar;
    float _speed;
    int _barToChange;
        
};


int main(int argc , char ** argv)
{
	Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex ); 

  Usul::CommandLine::Arguments::instance().set ( argc, argv );
	
	osg::ref_ptr< osg::Group > render_group ( new osg::Group() );

  osg::ref_ptr< OsgTools::Widgets::ProgressBarGroup > pbar ( new OsgTools::Widgets::ProgressBarGroup() );
  pbar->setLocation ( OsgTools::Widgets::ProgressBarGroup::LOWER_RIGHT );
  pbar->add ( std::string( "First Bar" ), 0, 100 );

  render_group->addChild( pbar->getProgressBarGroup() );

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
