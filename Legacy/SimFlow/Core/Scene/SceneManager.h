
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SCENE_SCENE_MANAGER_H__
#define __SCENE_SCENE_MANAGER_H__

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/DataObject/DataObject.h"
#include "SimFlow/Core/DataObject/Point.h"

#include "SimFlow/Core/Layer/PointLayer.h"

#include "osg/ref_ptr"
#include "osg/Geode"
#include "osg/Group"
#include "osg/Projection"

#include <map>

namespace SimFlow
{
	namespace Scene
	{
		class SIM_FLOW_EXPORT SceneManager : public osg::Referenced
		{
		public:

      typedef std::map< int, osg::ref_ptr< SimFlow::Layer::Layer > >	Layers;

			static SceneManager&				            instance();
			
			SceneManager();			

			void								                    buildScene();

			osg::Node*							                root() { return mRoot.get(); }

			const bool&							                dirty() const;
			void								                    dirty( const bool& );

      const bool&                             animate() const;
      void                                    animate( const bool& );

			void								                    addLayer( SimFlow::Layer::Layer *layer );
			void								                    removeLayer( int layerID );
			bool								                    hasLayer( int layerID ) const;
			SimFlow::Layer::Layer*			            getLayer( int layerID );
			unsigned int						                numberOfLayers();

			void								                    decrementTimeStep();
			void								                    incrementTimeStep();
			void								                    resetTimeStep();
			const unsigned int&					            getCurrentTimeStep() const;

			void								                    update();

		protected:
			virtual ~SceneManager()
			{
			}

		private:			

			bool									                  mDirty;

			bool									                  mUseInternalTimeStep;
			unsigned int							              mCurrentTimeStep;

      bool                                    mAnimate;

			osg::ref_ptr< osg::Group >	            mRoot;
			osg::ref_ptr< osg::Group >	            mStaticRoot;
			
			osg::ref_ptr< osg::Projection >			    mProjectionNode;
			osg::ref_ptr< osg::MatrixTransform >	  mDisplayPositionNode;
			osg::ref_ptr< osg::Geode >				      mDisplayNode;

			Layers									                mLayers;

			static SceneManager*					          mInstance;
		};
	}
}

#endif // __SCENE_SCENE_MANAGER_H__
