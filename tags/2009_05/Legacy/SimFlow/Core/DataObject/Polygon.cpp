
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Polygon.h"

using namespace SimFlow::DataObject;

Polygon::Polygon() : 
DataObject        (), 
mAutoScale			  ( false ), 
mAutoRotate			  ( false ), 
mMaterial			    ( new osg::Material() ), 
mMatrixTransform	( new osg::MatrixTransform() ), 
mAutoTransform		( new osg::AutoTransform() )
{
	mMaterial->setDiffuse  ( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.6, 0.6, 0.6, 1.0 ) );
	mMaterial->setSpecular ( osg::Material::FRONT_AND_BACK, osg::Vec4( 0.6, 0.6, 0.6, 1.0 ) );
	mMaterial->setShininess( osg::Material::FRONT_AND_BACK, 20.0 );
}

Polygon::~Polygon()
{
}

osg::Node* Polygon::buildScene()
{
	if( this->dirty() )
	{
		osg::ref_ptr< osg::StateSet > stateSet = mPolyData->getOrCreateStateSet();
		if( !stateSet.valid() ) 
		{ 
			stateSet = new osg::StateSet();
		}
		
		/// @Todo: Check here for the material else apply the material. 
    stateSet->setAttributeAndModes( mMaterial.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );

		mMatrixTransform->addChild( mAutoTransform.get() );		

		mAutoTransform->addChild( mPolyData.get() );

		mAutoTransform->setAutoScaleToScreen( mAutoScale );

		if( mAutoRotate )  
		{ 
			mAutoTransform->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN ); 
		}
		else  
		{ 
			mAutoTransform->setAutoRotateMode( osg::AutoTransform::NO_ROTATION );
		}		

		this->dirty( false );
	}

	return mMatrixTransform.get();
}

const bool& Polygon::autoRotate() const
{
	return mAutoRotate;
}

void Polygon::autoRotate( const bool& val )
{
	mAutoRotate = val;
	this->dirty( true );
}

const bool&	Polygon::autoScale() const
{
	return mAutoScale;
}

void Polygon::autoScale( const bool& val )
{
	mAutoScale = val;
	this->dirty( true );
}

osg::Node* Polygon::polyData() 
{
	return mPolyData.get();
}

void Polygon::polyData( osg::Node* polyData ) 
{
	mPolyData = polyData;
	this->dirty( true );
}

osg::Material* Polygon::material()
{
	return mMaterial.get();
}

void Polygon::material( osg::Material* material )
{
	mMaterial = material;
	this->dirty();
}

