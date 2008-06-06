
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __LAYER_LAYER_H__
#define __LAYER_LAYER_H__

#include "osg/Referenced"
#include "osg/ref_ptr"
#include "osg/MatrixTransform"
#include "osg/Material"

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/DataObject/DataObject.h"
#include "SimFlow/Core/DataObject/Point.h"

#include <vector>

namespace SimFlow
{
	namespace Layer
	{
		class SIM_FLOW_EXPORT Layer : public osg::Referenced
		{
		public: 
			
			typedef std::vector< osg::ref_ptr< SimFlow::DataObject::DataObject > >	DataObjects;

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

			Layer();

			virtual osg::Node*						        buildScene();

			osg::Material*							          material();
			void									                material( osg::Material* );

			// Get/Set dirty flag. 
			bool									                dirty() const;
			void									                dirty( bool );

			virtual unsigned int					        size() const;			

			const unsigned int&						        layerID() const;
			void									                layerID( const unsigned int& );

			const bool&								            show() const; 
			void									                show( const bool& );

      virtual const bool&                   temporal() const;
      virtual void                          temporal( const bool& );

			void									                addDataObject( DataObject::DataObject* );								
			
			void									                removeDataObject( const unsigned int& );

			void									                clearAllDataObject();

			DataObject::DataObject*				        getDataObject( const unsigned int& );

			const unsigned int&						        renderType() const;
			void									                renderType( const unsigned int& );

			const float&							            renderSize() const;
			void									                renderSize( const float& size );
			
			virtual void							            update(){}      

      virtual void                          animate( unsigned int ){}

		protected: 
			
			virtual ~Layer();			

		protected: 
			
			bool									                mDirty;			
			bool									                mShow;
      bool                                  mTemporal;

			unsigned int							            mLayerID;
			unsigned int							            mRenderType;

			float									                mRenderSize;

			osg::ref_ptr< osg::MatrixTransform >	mRoot;
			osg::ref_ptr< osg::Material >			    mMaterial;
			
			DataObjects								            mDataObjects;
		};
	}
}

#endif // __LAYER_LAYER_H__


