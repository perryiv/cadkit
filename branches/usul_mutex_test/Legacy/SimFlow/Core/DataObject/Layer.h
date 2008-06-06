#ifndef __CORE_LAYER_H__
#define __CORE_LAYER_H__

#include "Object.h"
#include "Model.h"

#include "Export.h"

namespace core
{
	class CORE_EXPORT Layer : public Object
	{
		public:
			typedef std::vector< osg::ref_ptr< Model > > Models;

			Layer() : Object()
			{
				try
				{
					mRoot = new osg::Group();
				}
				catch( char* error )
				{
					std::cerr << "[Sequence()] Exception raised: " << error << std::endl;
				}
			}

			Layer( const Layer& layer ) : Object( layer )
			{
				// Implement this. 
			}

			virtual void addModel( Model* model )
			{
				mModels.push_back( model );
			}

			virtual Model* getModel( const unsigned int& index )
			{
				assert( index < mModels.size() );
				
				return mModels[index].get();
			}

			virtual Models& getModels()
			{
				return mModels;
			}

			virtual osg::Group* getRoot()
			{
				return mRoot.get();
			}

			virtual void buildScene()
			{
				for( size_t i=0; i < mModels.size(); ++i )
				{
					mRoot->addChild( mModels[i]->getRoot() );
				}				
			}

		protected:

			virtual ~Layer()
			{
			}

		protected:

			osg::ref_ptr< osg::Group >	mRoot;			
	
			Models						mModels;
	};
}

#endif __CORE_LAYER_H__