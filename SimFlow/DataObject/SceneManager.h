///////////////////////////////////////////////////////////////////////////////
//
// This class has the responsibility of building the scene from a scene 
// description file. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CORE_SCENE_MANAGER_H__
#define __CORE_SCENE_MANAGER_H__

#include <osg/Referenced>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Sequence>
#include <osg/MatrixTransform>

#include <map>
#include <vector>
#include <cassert>

#include <Object.h>
#include <Scene.h>
#include <Export.h>

namespace core
{
	class CORE_EXPORT SceneManager : public Object
	{
		public:			
			
			// Easy to remember.
			typedef std::vector< osg::ref_ptr< Scene > > Scenes;

			SceneManager() : Object()
			{
				// Lets try to initialize. 
				try
				{
					mRoot = new osg::Group();
				}
				catch( char* error )
				{
					std::cerr << "[SceneManager] Exception raised: " << error << std::endl;
					abort();
				}
			}

			SceneManager( const SceneManager& sm ) : Object( sm )
			{
				// Implement this. 
			}

			virtual void addScene( Scene* scene )
			{
				mScenes.push_back( scene );	
			}

			virtual void removeScene( const unsigned int& index )
			{
				// Implement here.
			}	

			virtual Scene* getScene( const unsigned int& index )
			{
				assert( index < mScenes.size() );
				return mScenes[index].get();
			}

			virtual Scenes& getScenes() 
			{
				return mScenes;
			}

			virtual osg::Group* getRoot()
			{
				return mRoot.get();
			}

			virtual void buildScene()
			{
				for( size_t i=0; i < mScenes.size(); ++i )
				{
					mScenes[i]->buildScene();
					mRoot->addChild( mScenes[i]->getRoot() );
				}

			}

			virtual Scene* getLastScene()
			{
				return mScenes[ mScenes.size() - 1 ].get();
			}

			virtual void play()
			{
				// Implement here. 
			}

			virtual void pause()
			{
				// Implement here. 
			}

		protected:

			virtual ~SceneManager()
			{
			}
			
		protected:		
			
			osg::ref_ptr< osg::Group >	mRoot;

			Scenes						mScenes;		
	};
}

#endif // __CORE_SCENE_MANAGER_H__
