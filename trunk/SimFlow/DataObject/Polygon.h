
#ifndef __DATA_OBJECT_POLYGON_H__
#define __DATA_OBJECT_POLYGON_H__

#include "Export.h"
#include "DataObject.h"

#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/AutoTransform"

namespace SimFlow
{
	namespace DataObject 
	{
		class DATA_OBJECT_EXPORT Polygon : public DataObject 
		{
		public: 
			
			Polygon();			

			osg::Node*								            buildScene();

			const bool&								            autoRotate() const;
			void									                autoRotate( const bool& );

			const bool&								            autoScale() const;
			void									                autoScale( const bool& );
			
			osg::Node*								            polyData();
			void									                polyData( osg::Node* );

			osg::Material*						            material();
			void									                material( osg::Material* );
								
		protected:
			
			~Polygon();

		protected: 
				
			bool									                mAutoScale;
			bool									                mAutoRotate;			

			osg::ref_ptr< osg::Node	>				      mPolyData;
			osg::ref_ptr< osg::Material >			    mMaterial;
			osg::ref_ptr< osg::MatrixTransform >	mMatrixTransform;
			osg::ref_ptr< osg::AutoTransform  >		mAutoTransform;
		};
	}
}

#endif // __DATA_OBJECT_POLYGON_H__

