
#ifndef __LAYER_POINT_LAYER_H__
#define __LAYER_POINT_LAYER_H__

#include "Export.h"

#include "SimFlow/Layer/Layer.h"

#include "SimFlow/DataObject/Point.h"

namespace SimFlow
{
	namespace Layer
	{
		class LAYER_EXPORT PointLayer : public Layer
		{
		public: 

			PointLayer();			
			
		protected:

			virtual ~PointLayer()
			{
			}
		};
	}
}

#endif // __LAYER_POINT_LAYER_H__


