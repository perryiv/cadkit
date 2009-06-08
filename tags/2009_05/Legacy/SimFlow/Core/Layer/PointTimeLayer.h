
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// Container class to store PointLayers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __LAYER_POINT_TIME_LAYER_H__
#define __LAYER_POINT_TIME_LAYER_H__

#include "SimFlow/Core/Export.h"
#include "Layer.h"
#include "PointLayer.h"

#include <vector>

namespace SimFlow
{
	namespace Layer
	{
		class SIM_FLOW_EXPORT PointTimeLayer : public SimFlow::Layer::Layer
		{
		public:

			// Typedefs.
			typedef std::pair< unsigned int, osg::ref_ptr< PointLayer > >	TemporalPointLayer;
			typedef std::vector< TemporalPointLayer  >						        TemporalPointLayers;
			typedef std::vector< osg::ref_ptr< osg::MatrixTransform > >		TemporalTransforms;

			PointTimeLayer();

			virtual osg::Node*												buildScene();
			
			virtual void													    buildGeometry();

			virtual osg::Node*												getPrimitiveObject();	

			unsigned int													    size() const;

			void															        addTemporalPointLayer( const unsigned&, PointLayer* );

			TemporalPointLayer												getTemporalPointLayer( const unsigned& );
			
			TemporalPointLayers												getTemporalPointLayers();		

      virtual void                              animate( unsigned int );

		protected: 

			virtual ~PointTimeLayer()
			{
			}

			unsigned int													    maxSize();			

		protected:

			unsigned int													    mMaxSize;			

			TemporalPointLayers												mTemporalPointLayers;			
			TemporalTransforms												mTemporalTransforms;

			osg::ref_ptr< osg::Geode >								mPrimitiveObject;
		};
	}
}

#endif // __LAYER_POINT_TIME_LAYER_H__


