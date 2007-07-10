
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

using namespace SimFlow::Scene;

SceneManager* SceneManager::mInstance( 0x00 );

SceneManager& SceneManager::instance()
{
	if( 0x00 == mInstance )
	{
		mInstance = new SceneManager();
	}

	return *mInstance;
}

SceneManager::SceneManager() : 
mDirty					      ( true ),
mUseInternalTimeStep	( true ),
mCurrentTimeStep		  ( 0 ),
mAnimate              ( true ), 
mRoot					        ( new osg::Group() ), 
mStaticRoot				    ( new osg::Group() ),
mProjectionNode			  ( new osg::Projection() ), 
mDisplayPositionNode	( new osg::MatrixTransform() ),
mDisplayNode			    ( new osg::Geode() )
{
	mDisplayPositionNode->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

	mDisplayPositionNode->addChild( mDisplayNode.get() );

	mProjectionNode->addChild( mDisplayPositionNode.get() );

	mLayers.clear();
}

void SceneManager::buildScene()
{
	if( this->dirty() )
	{
		mRoot->removeChild( 0, mRoot->getNumChildren() );

		mStaticRoot->removeChild( 0, mStaticRoot->getNumChildren() );

		for( Layers::iterator itr = mLayers.begin(); itr != mLayers.end(); ++itr )
		{
			mRoot->addChild( itr->second->buildScene() );
		}

		mRoot->addChild( mStaticRoot.get() );
	}
}

const bool& SceneManager::dirty() const 
{
	return mDirty;
}

void SceneManager::dirty( const bool& val  )
{
	mDirty = true;
}

const bool& SceneManager::animate() const
{
  return mAnimate;
}

void SceneManager::animate( const bool& val )
{
  mAnimate = val;
}

void SceneManager::addLayer( SimFlow::Layer::Layer *layer )
{
	mLayers[ layer->layerID() ] = layer;	
	this->dirty( true );
}

void SceneManager::removeLayer( int layerID )
{
	Layers::iterator iter = mLayers.find ( layerID );
	if ( iter != mLayers.end() )
	{
		mLayers.erase ( iter );
		this->dirty ( true );
	}
}

bool SceneManager::hasLayer( int layerID ) const
{
	Layers::const_iterator iter = mLayers.find( layerID );
	if( iter != mLayers.end() )
		return true;
	return false;
}

SimFlow::Layer::Layer* SceneManager::getLayer( int layerID )
{
	Layers::iterator iter = mLayers.find ( layerID );

	if( iter != mLayers.end() )
	{
		return iter->second.get();
	}
	else
	{
		return 0x00;
	}
}

unsigned int SceneManager::numberOfLayers() 
{
	return mLayers.size();
}

void SceneManager::decrementTimeStep()
{
	if( mCurrentTimeStep != 0 ){ --mCurrentTimeStep; }
}

void SceneManager::incrementTimeStep()
{
	++mCurrentTimeStep;	
}

void SceneManager::resetTimeStep()
{
	mCurrentTimeStep = 0;
}

const unsigned int& SceneManager::getCurrentTimeStep() const
{
	return mCurrentTimeStep;
}

void SceneManager::update()
{
	for( Layers::iterator itr = mLayers.begin(); itr != mLayers.end(); ++itr )
	{
		itr->second->update();
  
    if( mAnimate && itr->second->temporal() )
    {
      if( mUseInternalTimeStep )
      { 
        incrementTimeStep(); 
      }
      itr->second->animate( getCurrentTimeStep() );
    }
	}
}
