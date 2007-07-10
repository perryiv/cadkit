
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __LAYER_POINT_LAYER_H__
#define __LAYER_POINT_LAYER_H__

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/Layer/Layer.h"

#include "SimFlow/Core/DataObject/Point.h"

namespace SimFlow
{
	namespace Layer
	{
		class SIM_FLOW_EXPORT PointLayer : public Layer
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


