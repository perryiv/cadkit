
#include "osgViewer/Viewer"

#include "Cinema/Core/Movie.h"

int main(int argc, char** argv)
{    
	if ( argc < 2 )
	{
		std::cout << "No file specified." << std::endl
			<< "Usage: " << argv[0] << " filename" << std::endl;
	}

  // construct the viewer.
  osgViewer::Viewer viewer;

  osg::ref_ptr< Cinema::Core::Movie > movie = Cinema::Core::Movie::instance();
  movie->setMovie( osg::Vec3f(), osg::Vec3f( 10.0, 0.0, 0.0 ), osg::Vec3f( 0.0, 0.0, 10.0 ), argv[1] );
  movie->buildScene();
  //movie->play(); 

  // set the scene to render
  viewer.setSceneData ( movie->root() );

  // create the windows and run the threads.
  viewer.realize();

	const int result ( viewer.run() );

  movie->pause();

  return result;
}
