
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DATA_OBJECT_DATA_OBJECT_H__
#define __DATA_OBJECT_DATA_OBJECT_H__

#include "SimFlow/Core/Export.h"

#include "osg/Referenced"
#include "osg/Vec3"
#include "osg/Vec4"

#include <string>

namespace osg { class Node; }

namespace SimFlow
{
	namespace DataObject
	{
		class SIM_FLOW_EXPORT DataObject : public osg::Referenced
		{
		public:

			/// Constructor. 
			DataObject();
		
			/// Build the branch for this data object. 
			virtual osg::Node* buildScene() = 0;

			/// Get/Set color of the object. 
			const unsigned int&		color() const;
			void					color( const unsigned int& );

			/// Get/Set dirty flag. 
			bool					dirty() const;
			void					dirty( bool );

			/*
			/// Get/Set name id. 
			const std::string		label() const;
			void					label( const std::string& );
			
			/// Get/Set label position. 
			const osg::Vec3&		labelPosition() const;
			void					labelPosition( const osg::Vec3& );

			/// Get/Set tooltip. 
			const std::string&		toolTip() const;
			void					toolTip( const std::string& );
			*/

		protected:

			/// Object can be created only on the heap. 
			virtual ~DataObject();

		protected:			

			unsigned int			mColor;
			bool					mDirty;
			//std::string				mLabel;
			//osg::Vec3				mLabelPosition;
			//std::string				mToolTip;
		};
	}
}

#endif // __DATA_OBJECT_DATA_OBJECT_H__
