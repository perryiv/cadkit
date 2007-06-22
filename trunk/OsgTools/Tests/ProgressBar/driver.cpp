
#include "Threads/OpenThreads/Mutex.h"
#include "Threads/OpenThreads/Thread.h"

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>


#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Math>


#include "terrain_coords.h" 

#include "Usul/CommandLine/Arguments.h"

#include "OsgTools/Widgets/ProgressBarGroup.h"


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
              case(osgGA::GUIEventAdapter::PUSH):
				      {
      					
					      //_compass->setMatrix(_viewer->getCamera()->getViewMatrix());
      					
					      return false;
				      }
				      case(osgGA::GUIEventAdapter::RELEASE):
				      {
					      return false;
				      }
				      case(osgGA::GUIEventAdapter::DRAG):
				      {
					      if(ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
					      {
      						

      						
					      }
					      return false;
				      }
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

osg::Geode* createShapes()
{
	osg::ref_ptr<osg::Geode> geode (new osg::Geode());
    //osg::Geode* geode = new osg::Geode();

    // ---------------------------------------
    // Set up a StateSet to texture the objects
    // ---------------------------------------
	osg::ref_ptr<osg::StateSet> stateset (new osg::StateSet());
    //osg::StateSet* stateset = new osg::StateSet();

	osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/lz.rgb" );
    //osg::Image* image = osgDB::readImageFile( "Images/lz.rgb" );
	
    if (image.get())
    {
		osg::ref_ptr<osg::Texture2D> texture (new osg::Texture2D()); 
        //osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image.get());
		stateset->setTextureAttributeAndModes(0,texture.get(),osg::StateAttribute::ON);
    }
    
    geode->setStateSet( stateset.get() );
    
    float radius = 0.8f;
    float height = 1.0f;
    
	osg::ref_ptr<osg::TessellationHints> hints (new osg::TessellationHints());
    //osg::TessellationHints* hints = new osg::TessellationHints;
    hints->setDetailRatio(0.5f);

	osg::ref_ptr<osg::HeightField> grid (new osg::HeightField());
    //osg::HeightField* grid = new osg::HeightField;
    grid->allocate(38,39);
    grid->setXInterval(0.28f);
    grid->setYInterval(0.28f);
    
    for(unsigned int r=0;r<39;++r)
    {
        for(unsigned int c=0;c<38;++c)
        {
            grid->setHeight(c,r,vertex[r+c*39][2]);
        }
    }
    geode->addDrawable(new osg::ShapeDrawable(grid.get()));
    
    
    return geode.release();
	//return geode;
}
int main(int argc , char ** argv)
{
	Usul::Threads::Mutex::createFunction ( &Threads::OT::newOpenThreadsMutex );
  
  // construct the viewer.
	osg::ref_ptr< osgViewer::Viewer > viewer ( new osgViewer::Viewer() );
	osg::ref_ptr< osg::Group > render_group ( new osg::Group() );
  osg::ref_ptr< OsgTools::Widgets::ProgressBarGroup > pbar ( new OsgTools::Widgets::ProgressBarGroup() );
  /*pbar->setPosition( osg::Vec3f ( ( pbar->getPosition().x() - pbar->getLength() / 2 ),
                                  pbar->getPosition().y() - 0.3f,
                                  pbar->getPosition().z() ) );*/

  //pbar->setNumItems( 20 );
  pbar->setLocation ( OsgTools::Widgets::ProgressBarGroup::LOWER_RIGHT );
  pbar->add ( std::string( "First Bar" ), 0, 100 );
  

  viewer->addEventHandler( new DriverEventHandler( pbar.get() ) );
  
  render_group->addChild( pbar->getProgressBarGroup() );
	//render_group->addChild( createShapes() );

	viewer->setSceneData( render_group.get() );
  //osgDB::writeNodeFile( *viewer->getSceneData(), "debug.osg" );

  int result (  viewer->run() );

  viewer->setSceneData ( 0x0 );

  return result;
	
	
}