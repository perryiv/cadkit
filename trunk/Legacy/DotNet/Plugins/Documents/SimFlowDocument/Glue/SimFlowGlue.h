
#pragma once

#include "Usul/Strings/Managed.h"

#include "osg/Group"
#include "osg/Node"
#include "osgDB/ReadFile"

#include "Scene/SceneManager.h"
#include "App/Parser.h"
#include "App/AppSAX2Handler.h"

namespace CadKit 
{
	namespace Plugins
	{
		namespace Documents
		{
			namespace SimFlowDocument
			{
				public ref class SimFlowGlue 
				{
					public:
            SimFlowGlue( System::String^ fileName, System::Object^ caller )
						{
							// Create a scene manager. 
              mSceneManager = &( SimFlow::Scene::SceneManager::instance() );

							// App specific handler for XML. 
							SimFlow::App::AppSAX2Handler* handler = new SimFlow::App::AppSAX2Handler();
							handler->sceneManager( mSceneManager );

							// Parser object. 
							SimFlow::App::Parser* parser = new SimFlow::App::Parser();
							parser->init();
							parser->setHandler( handler );

							// Now parse the file.
              parser->readXML ( Usul::Strings::convert ( fileName ) );

							//parser->readXML( "scene.xml" );
							mSceneManager->buildScene();

							mSceneRoot = new osg::Group();
							mSceneRoot->addChild( mSceneManager->root() );
							/*
							
							osg::Node* node = osgDB::readNodeFile( "C:\\Aashish\\Projects\\LitchFieldPark\\data\\LP-OSG\\Rosewood\\Rosewood.osg" );
							mSceneRoot->addChild( node );
							*/

              update();
						}

						CadKit::OSG::Glue::Node ^root()
						{
							return gcnew CadKit::OSG::Glue::Node ( System::IntPtr ( mSceneRoot ) );
						}

            void update()
            {
              mSceneManager->update();
            }

					private: 

						osg::Group*		mSceneRoot;

            SimFlow::Scene::SceneManager* mSceneManager;
				};
			}
		}
	}
}