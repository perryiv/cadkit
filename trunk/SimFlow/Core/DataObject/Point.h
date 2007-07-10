
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_POINT_H__
#define __DATA_OBJECT_POINT_H__

#include "SimFlow/Core/Export.h"
#include "DataObject.h"

#include "osg/Material"
#include "osg/AutoTransform"

namespace SimFlow
{
	namespace DataObject
	{
		class SIM_FLOW_EXPORT Point : public DataObject
		{
		public:				
			
			enum RenderType
			{
				POINT = 1, 
				SPHERE, 
				BOX, 
				CONE, 
				CYLINDER, 
				ELLIPSOID, 
				CUSTOM 
			};

			Point(); 
				
			osg::Node*							              buildScene();

			osg::Node*							              buildGeometry();

			const float&						              size() const;
			void								                  size( const float& );

			const osg::Vec3&			                center() const;
			void								                  center( const osg::Vec3& );

			const unsigned int&			              renderType() const;
			void								                  renderType( const unsigned int& );

			//osg::Material*				              material();
			//void								                material( osg::Material* );
			
		protected: 
			
			~Point();

		protected:
			
			float								                  mSize;
			osg::Vec3							                mCenter;
			unsigned int						              mRenderType;

			//osg::ref_ptr< osg::Material >		    mMaterial;	
			//osg::ref_ptr< osg::AutoTransform >	mTransform;
		};
	}
}

#endif // __DATA_OBJECT_POINT_H__

