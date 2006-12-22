
#pragma once

#include "Usul/Strings/ManagedToNative.h"

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
						SimFlowGlue( System::String^ fileName )
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
              System::IntPtr ptr = System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( fileName );
              char* s = (char*)(void*) ptr;
							
              //parser->readXML( Usul::Strings::convert( fileName ) );
              parser->readXML( s );

              System::Runtime::InteropServices::Marshal::FreeHGlobal( ptr );
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
							CadKit::OSG::Glue::Node ^temp = gcnew CadKit::OSG::Glue::Node() ;
							temp->nodePtr( System::IntPtr( mSceneRoot ) );
							return temp;
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