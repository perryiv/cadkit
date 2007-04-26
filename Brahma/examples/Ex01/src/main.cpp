
#include "Brahma/Core/Brahma.h"

#include "Brahma/Scene/Node.h"
#include "Brahma/Scene/Group.h"
#include "Brahma/Scene/Transform.h"
#include "Brahma/Scene/LeafNode.h"

#include "Parser/Scene/SceneNodeVisitor.h"

#include "Usul/Threads/Mutex.h"

#include "osg/Node"
#include "osg/Group"
#include "osg/MatrixTransform"

#include "osgProducer/Viewer"
#include "osg/CoordinateSystemNode"

#include "osgDB/ReadFile"

const std::string INPUT_FILE_NAME( "input.xml" );
const std::string OUTPUT_FILE_NAME( "output.xml" );

Usul::Threads::SetMutexFactory factory ( &Usul::Threads::newSingleThreadedMutexStub );

#if 0
void addNode( Brahma::Scene::Node* node, osg::Group* osgGroup )
{   
  Brahma::Scene::LeafNode::RefPtr b( node->database() );
  if( db.valid() )
  {
    // Check here whether we have a dir or a file. 
    osg::ref_ptr< osg::Node > osgNode = osgDB::readNodeFile( db->data() );
    
    if( osgNode.valid() )
    {
      osgNode->setName( node->id() );
      osgGroup->addChild( osgNode.get() );
    }
  }
}

void addChildren( Brahma::Scene::Group* group, osg::Group* osgGroup )
{
  Brahma::Scene::Group::Children children = group->children();

  for( size_t i = 0; i < children.size();  ++i )
  {    
    
    Brahma::Scene::Group::RefPtr subGroup = dynamic_cast< Brahma::Scene::Group* >( children[ i ].get() );
    if( subGroup.valid() )
    {
      osg::ref_ptr< osg::Group > osgSubGroup( new osg::Group() );
      
      if( osgSubGroup.valid() )
      {
        osgSubGroup->setName( subGroup->id() );
        osgGroup->addChild( osgSubGroup.get() );        
        addChildren( subGroup.get(), osgSubGroup.get() );
      }
      else
      {
        return;
      }
    }

    Brahma::Scene::Transform::RefPtr transform = dynamic_cast< Brahma::Scene::Transform* >( children[ i ].get() );
    if( transform.valid() )
    {
      osg::ref_ptr< osg::MatrixTransform > osgTransform( new osg::MatrixTransform() );
      
      if( osgTransform.valid() )
      {
        osgTransform->setName( transform->id() );
        osgGroup->addChild( osgTransform.get() );
        addChildren( transform.get(), osgTransform.get() );
      }
      else
      {
        return;
      }
    }

    Brahma::Scene::Node::RefPtr node = dynamic_cast< Brahma::Scene::Node* >( children[ i ].get() );
    if( node.valid() )
    {
      // Leaf node.
      addNode( node.get(), osgGroup );
    }
    else
    {
      return;
    }
  }
}

void parse( const Brahma::Core::Brahma::BaseObjects& objects, osg::Group* osgRoot )
{
  for( size_t i = 0; i < objects.size(); ++i )
  {
    Brahma::Scene::Group::RefPtr group = dynamic_cast< Brahma::Scene::Group* >( objects[ i ].get() );
    
    if( group.valid() && osgRoot )
    {
      addChildren( group.get(), osgRoot );
    }

    Brahma::Scene::Node::RefPtr node = dynamic_cast< Brahma::Scene::Node* >( objects[ i ].get() );
    
    if( node.valid() && osgRoot )
    {
      addNode( node.get(), osgRoot );
    }
  }
}
#endif 

int main( int argc, char** argv )
{
  osg::ref_ptr< osg::Group > osgRoot( new osg::Group() );

	Parser::Scene::SceneNodeVisitor::RefPtr nodeVisitor( new Parser::Scene::SceneNodeVisitor() );

  Brahma::Core::Brahma::RefPtr brahma( new Brahma::Core::Brahma() );

  //brahma->registerType< Brahma::Scene::Node > ( "Node" );
  brahma->registerType< Brahma::Scene::Group >( "Group" );
  brahma->registerType< Brahma::Scene::Transform >( "Transform" );
  brahma->registerType< Brahma::Scene::LeafNode >( "LeafNode" );

  brahma->load( INPUT_FILE_NAME );

  brahma->deserialize();

	brahma->serialize( OUTPUT_FILE_NAME );

	#if 0
  parse( brahma->objects(), osgRoot.get() );       
	#endif

	Brahma::Core::Brahma::BaseObjects objects( brahma->objects() );

	for( size_t i = 0; i < objects.size(); ++i )
  {
    Brahma::Scene::Group::RefPtr group = dynamic_cast< Brahma::Scene::Group* >( objects[ i ].get() );
    
    if( group.valid() )
    {
			group->accept( *( nodeVisitor.get() ) );
    }

   // Brahma::Scene::Node::RefPtr node = dynamic_cast< Brahma::Scene::Node* >( objects[ i ].get() );
   // 
   // if( node.valid() )
   // {
			//node->accept( *( nodeVisitor.get() ) );
   // }
  }

  //brahma->serialize( OUTPUT_FILE_NAME );

  // use an ArgumentParser object to manage the program arguments.
  osg::ArgumentParser arguments(&argc,argv);
  
  // construct the viewer.
  osgProducer::Viewer viewer( arguments );

  // set up the value with sensible default event handlers.
  viewer.setUpViewer( osgProducer::Viewer::STANDARD_SETTINGS );  
  
  // Start time counter. 
  osg::Timer_t start_tick = osg::Timer::instance()->tick();
  
  // any option left unread are converted into errors to write out later.
  arguments.reportRemainingOptionsAsUnrecognized();

  // report any errors if they have occurred when parsing the program arguments.
  if ( arguments.errors() )
  {
      arguments.writeErrorMessages( std::cout );
  }

  osg::Timer_t end_tick = osg::Timer::instance()->tick();

  std::cout << "Time to load = "<<osg::Timer::instance()->delta_s(start_tick,end_tick)<<std::endl;

  // pass the loaded scene graph to the viewer.
  if( !osgRoot.valid() )
  {
    return 1;
  }

	#if 0
  viewer.setSceneData( osgRoot.get() );
	#endif

	viewer.setSceneData( nodeVisitor->root() );

  // create the windows and run the threads.
  viewer.realize();

  while( !viewer.done() )
  {
      // wait for all cull and draw threads to complete.
      viewer.sync();

      // update the scene by traversing it with the the update visitor which will
      // call all node update callbacks and animations.
      viewer.update();
       
      // fire off the cull and draw traversals of the scene.
      viewer.frame();
      
  }
  
  // wait for all cull and draw threads to complete.
  viewer.sync();

  // run a clean up frame to delete all OpenGL objects.
  viewer.cleanup_frame();

  // wait for all the clean up frame to complete.
  viewer.sync();

  return 0;
}
