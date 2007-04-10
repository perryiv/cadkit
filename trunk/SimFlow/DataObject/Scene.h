#ifndef __CORE_SCENE_H__
#define __CORE_SCENE_H__

#include "Layer.h"
#include "Sequence.h"

#include "Export.h"

namespace core
{
	class CORE_EXPORT Scene : public Object
	{
		public:
			typedef std::vector< osg::ref_ptr< Layer > >	Layers;
			typedef std::vector< osg::ref_ptr< Sequence > > Sequences;

			Scene() : Object()
			{
				try
				{
					mRoot = new osg::Group();
				}
				catch( char* error )
				{
					std::cerr << "[Scene] Exception raised: " << error << std::endl;
				}
			}		

			Scene( const Scene& scene  ) : Object( scene )
			{
				// Implement this. 
			}

			virtual void addLayer( Layer* layer )
			{
				mLayers.push_back( layer );	
			}

			virtual void addSequence( Sequence* seq )
			{
				mSequences.push_back( seq );				
			}

			virtual Layers getLayers()
			{
				return mLayers;
			}

			virtual Sequences getSequences()
			{
				return mSequences;
			}

			virtual osg::Group* getRoot()
			{
				return mRoot.get();
			}

			virtual void buildScene()
			{
				for( size_t i=0; i < mLayers.size(); ++i )
				{
					mLayers[i]->buildScene();
					mRoot->addChild( mLayers[i]->getRoot() );
				}

				for( size_t i=0; i < mSequences.size(); ++i )
				{
					mSequences[i]->buildScene();
					mRoot->addChild( mSequences[i]->getRoot() );
				}				
			}

			virtual Sequence* getLastSequence()
			{
				return mSequences[ mSequences.size() - 1 ].get();
			}

			virtual Layer* getLastLayer()
			{
				return mLayers[ mLayers.size() - 1 ].get();
			}		

		protected:

			virtual ~Scene()
			{
			}

		protected:		

			osg::ref_ptr< osg::Group >	mRoot;

			Layers						mLayers;

			Sequences					mSequences;
	};
}

#endif __CORE_SCENE_H__

