
#ifndef __LAYER_POLYGON_LAYER_H__
#define __LAYER_POLYGON_LAYER_H__

#include "Export.h"

#include "SimFlow/DataObject/Polygon.h"

#include "SimFlow/Layer/Layer.h"

#include "osg/Material"

namespace SimFlow
{
	namespace Layer
	{
		class LAYER_EXPORT PolygonLayer : public Layer
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


