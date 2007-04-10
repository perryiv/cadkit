#ifndef __CORE_OBJECT_H__
#define __CORE_OBJECT_H__

#include <osg/Referenced>

#include "Export.h"

namespace core
{
	class CORE_EXPORT Object : public osg::Referenced
	{
		public:
			Object() :
				mID("")
			{
			}

			Object( const Object& object ) :
				mID( object.mID )
			{
			}

			std::string getID() const 
			{
				return mID;
			}

			void setID( const std::string& id )
			{
				mID = id;
			}

		protected:
			virtual ~Object()
			{
			}

		protected:
			std::string mID;
	};
}

#endif // __CORE_OBJECT_H__