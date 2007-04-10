
#include "SceneManager.h"

using namespace SimFlow::Scene;

osg::ref_ptr< SceneManager > SceneManager::mInstance( 0x00 );

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
  mAbsoluteGroup				( new osg::MatrixTransform() ),  
  mReferenceGroup       ( new osg::MatrixTransform() ),
  mModelGroup           ( new osg::Group() ),
  mProjectionGroup	    ( new osg::Projection() )  
{
  mAbsoluteGroup->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

	mLayers.clear();
}

void SceneManager::buildScene()
{
	if( this->dirty() )
	{
		mRoot->removeChild( 0, mRoot->getNumChildren() );		

		for( Layers::iterator itr = mLayers.begin(); itr != mLayers.end(); ++itr )
		{
			mModelGroup->addChild( itr->second->buildScene() );
		}

		mRoot->addChild( mAbsoluteGroup.get() );
    mRoot->addChild( mReferenceGroup.get() );
    mRoot->addChild( mProjectionGroup.get() );

    mReferenceGroup->addChild( mModelGroup.get() );
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
