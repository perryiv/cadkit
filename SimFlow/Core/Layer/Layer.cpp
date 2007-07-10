
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Layer.h"

using namespace SimFlow::Layer;

Layer::Layer() :
osg::Referenced (), 
mDirty          ( false ),
mShow           ( true ),
mTemporal       ( false ), 
mLayerID        (), 
mRenderType     ( 1 ), 
mRenderSize     ( 1.5 ), 
mRoot           ( new osg::MatrixTransform() ), 
mMaterial       ( new osg::Material() )
{
	//mMaterial->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4f( 0.01, 0.01, 0.01, 1.0 ) );
	mMaterial->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4f( 0.9, 0.4, 0.05, 1.0 ) );
	mMaterial->setSpecular( osg::Material::FRONT_AND_BACK, osg::Vec4f( 0.6, 0.6, 0.6, 1.0 ) );
	mMaterial->setShininess( osg::Material::FRONT_AND_BACK, 20.0 );
}

Layer::~Layer()
{
}

bool Layer::dirty() const 
{
	return mDirty;
}

void Layer::dirty( bool val )
{
	mDirty = val;
}

unsigned int Layer::size() const
{
	return mDataObjects.size();
}

osg::Node* Layer::buildScene()
{
	for( DataObjects::iterator itr = mDataObjects.begin(); itr != mDataObjects.end(); ++itr )
	{
		mRoot->addChild( (*itr)->buildScene() );
	}

	return mRoot.get();
}

osg::Material* Layer::material()
{
	return mMaterial.get();
}

void Layer::material( osg::Material* mat )
{
	mMaterial = mat;
}

const unsigned int& Layer::layerID() const
{
	return mLayerID;
}

void Layer::layerID( const unsigned int& layerID )
{
	mLayerID = layerID;
}

const bool& Layer::show() const
{
	return mShow;
}

void Layer::show( const bool&  val )
{
	mShow = val;
}

const bool& Layer::temporal() const
{
  return mTemporal;
}

void Layer::temporal( const bool& val )
{
  mTemporal = val;
}

void Layer::addDataObject( SimFlow::DataObject::DataObject* data )
{
	mDataObjects.push_back( data );
}
			
void Layer::removeDataObject( const unsigned int& index )
{
	/// @Todo: Implement this. 
}

void Layer::clearAllDataObject()
{
	/// @Todo: Implement this. 
}

SimFlow::DataObject::DataObject* Layer::getDataObject( const unsigned int& index )
{
	return mDataObjects[index].get();
}

const unsigned int& Layer::renderType() const
{
	return mRenderType;
}

void Layer::renderType( const unsigned int& renderType )
{
	mRenderType = renderType;
	this->dirty( true );
}

const float& Layer::renderSize() const
{
	return mRenderSize;
}

void Layer::renderSize( const float& size )
{
	mRenderSize = size;
}


