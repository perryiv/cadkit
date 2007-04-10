#ifndef __CORE_MODEL_H__
#define __CORE_MODEL_H__

#include "Object.h"
#include "Export.h"

namespace core
{
	class CORE_EXPORT Model : public Object
	{
		public:

			Model() : Object()
			{		
			}

			Model( const Model& model ) : Object( model )
			{
			}

			virtual void setData( osg::Node* data )
			{
				mData = data;
			}	

			virtual osg::Node* getRoot()
			{
				return mData.get();
			}

		protected:

			virtual ~Model()
			{
			}

		protected:
			
			osg::ref_ptr< osg::Node >	mData;
	};
}

#endif // __CORE_MODEL_H__