
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_SCENE_MANAGER_H__
#define __SCENE_SCENE_MANAGER_H__

#include "Export.h"

#include "SimFlow/DataObject/DataObject.h"
#include "SimFlow/DataObject/Point.h"

#include "SimFlow/Layer/PointLayer.h"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Group"
#include "osg/Projection"

#include <map>

namespace SimFlow
{
	namespace Scene
	{
		class SCENE_EXPORT SceneManager : public osg::Referenced
		{
		public:

      typedef std::map< int, osg::ref_ptr< SimFlow::Layer::Layer > >	Layers;

			static SceneManager&                    instance();
			
			SceneManager();			

			virtual void				                    buildScene();

			osg::Node*							                root() { return mRoot.get(); }

			const bool&							                dirty() const;
			void								                    dirty( const bool& );

      const bool&                             animate() const;
      void                                    animate( const bool& );

			virtual void				                    addLayer( SimFlow::Layer::Layer *layer );
			virtual void				                    removeLayer( int layerID );
			virtual bool				                    hasLayer( int layerID ) const;
			virtual SimFlow::Layer::Layer*          getLayer( int layerID );
			virtual unsigned int		                numberOfLayers();

			void								                    decrementTimeStep();
			void								                    incrementTimeStep();
			void								                    resetTimeStep();
			const unsigned int&					            getCurrentTimeStep() const;

			virtual void				                    update();

		protected:

			virtual ~SceneManager()
			{
			}

		protected:			

			bool									                  mDirty;

			bool									                  mUseInternalTimeStep;
			unsigned int							              mCurrentTimeStep;

      bool                                    mAnimate;

			osg::ref_ptr< osg::Group >	            mRoot;

      osg::ref_ptr< osg::MatrixTransform >    mAbsoluteGroup;
  
      osg::ref_ptr< osg::MatrixTransform >    mReferenceGroup;
			
      osg::ref_ptr< osg::Group >              mModelGroup;

      osg::ref_ptr< osg::Projection >			    mProjectionGroup;			
      
			
      osg::ref_ptr< osg::Geode >				      mDisplayNode;

			Layers									                mLayers;

			static osg::ref_ptr< SceneManager >     mInstance;
		};
	}
}

#endif // __SCENE_SCENE_MANAGER_H__
