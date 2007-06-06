
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/Image>
#include <osgViewer/Viewer>


#include <osgDB/ReadFile>

#include <osg/Math>

#include "CompassEventHandler.h"
#include "terrain_coords.h"
#include "OsgTools/Builders/Compass.h"


class DriverEventHandler : public osgGA::GUIEventHandler
{
public:
    
	DriverEventHandler(Compass* c, osgViewer::Viewer* v):
             _compass(c),_viewer(v){}
    
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
						//_compass->setMatrix(_viewer->getCamera()->getViewMatrix());

						
					}
					return false;
				}

                default:
                    return false;
            }
        }
        
		osg::ref_ptr<Compass> _compass;
		osg::ref_ptr<osgViewer::Viewer> _viewer;
        
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
int main(int, char **)
{
    // construct the viewer.
	
	osg::ref_ptr<osgViewer::Viewer> viewer (new osgViewer::Viewer());
	//osg::Matrixd* vm = new osg::Matrixd(viewer->getCamera()->getViewMatrix());
	//osg::ref_ptr<Compass> compass ( new Compass( vm ) );
	osg::ref_ptr<OsgTools::Builders::Compass> compass ( new OsgTools::Builders::Compass() );
	//osg::ref_ptr<osg::Matrix> viewing_matrix (new osg::Matrix( viewer->getCamera()->getViewMatrix()));
	

	//viewer->addEventHandler(new DriverEventHandler(compass.get(), viewer.get()));
	viewer->addEventHandler( new CompassEventHandler( compass.get() ) );
	osg::ref_ptr<osg::Group> render_group (new osg::Group());
	render_group->addChild( compass->getCompass() );
	render_group->addChild(createShapes());

	viewer->setSceneData(render_group.get());
	//viewer.setSceneData( compass->getCompass() );
	//viewer.setSceneData( createShapes());
	

    return viewer->run();
	
	
}