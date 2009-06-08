
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __LAYER_POLYGON_LAYER_H__
#define __LAYER_POLYGON_LAYER_H__

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/DataObject/Polygon.h"

#include "SimFlow/Core/Layer/Layer.h"

#include "osg/Material"

namespace SimFlow
{
	namespace Layer
	{
		class SIM_FLOW_EXPORT PolygonLayer : public Layer
		{
		public: 

			enum AttributrMode
			{				
				DONOT_OVERRIDE	= 1,
				OVERRIDE		= 2
			};

			PolygonLayer();				

			const unsigned int&						attributeMode() const;
			void									        attributeMode( const unsigned int& );

			const bool&								    autoRotate() const;
			void									        autoRotate( const bool& );

			const bool&								    autoScale() const;
			void									        autoScale( const bool& );
			
		protected:
			
			virtual ~PolygonLayer();

		protected:

			unsigned int							    mAttributeMode;
			bool									        mAutoScale;
			bool									        mAutoRotate;						
		};
	}
}

#endif // __LAYER_POLYGON_LAYER_H__


