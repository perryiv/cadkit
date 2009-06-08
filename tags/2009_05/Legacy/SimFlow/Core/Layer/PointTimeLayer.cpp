
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "SimFlow/Core/Layer/PointTimeLayer.h"

#include "OsgTools/ShapeFactory.h"

#include <cassert>
#include <stdexcept>

#include "osg/Geode"
#include "osg/Node"

using namespace SimFlow::Layer;

PointTimeLayer::PointTimeLayer() :
Layer       (), 
mMaxSize    ( 0 ) 
{
  temporal( true );
}

osg::Node* PointTimeLayer::buildScene()
{
	mMaxSize = maxSize();

	buildGeometry();

	for( size_t i=0; i < mMaxSize; ++i )
	{	
		osg::ref_ptr< osg::MatrixTransform > mt ( new osg::MatrixTransform() );
		mt->addChild( getPrimitiveObject() );

		osg::ref_ptr< osg::StateSet > ss( mt->getOrCreateStateSet() );
		ss->setAttributeAndModes( material(), osg::StateAttribute::ON );

		//osg::ref_ptr< SimFlow::DataObject::Point > pt = dynamic_cast< SimFlow::DataObject::Point* > ( mTemporalPointLayers[0].second->getDataObject( i ) );
		//mt->setMatrix( osg::Matrix::translate( pt->center() ) );
		mRoot->addChild( mt.get() );
		mTemporalTransforms.push_back( mt.get() );		
	}

	return mRoot.get();
}

void PointTimeLayer::buildGeometry()
{
	switch( renderType() )
	{
		case 1:
		{
			osg::ref_ptr< osg::Geode > geode( new osg::Geode() );
      OsgTools::ShapeFactory::MeshSize size ( 10, 10 );
      osg::ref_ptr < osg::Geometry > geometry ( OsgTools::ShapeFactorySingleton::instance().sphere ( this->renderSize(), size ) );

      geode->addDrawable( geometry.get() );
			
			mPrimitiveObject = geode.get();
			break;			
		}
		default:
		{
			throw std::runtime_error( std::string( "[ERROR:ERRORNO] Render type not handled " ) );
			break;
		}
	};
}

osg::Node* PointTimeLayer::getPrimitiveObject()
{
	return mPrimitiveObject.get();
}

unsigned int PointTimeLayer::size() const
{
	return mTemporalPointLayers.size();
}

void PointTimeLayer::addTemporalPointLayer( const unsigned& id, PointLayer* pl )
{
	mTemporalPointLayers.push_back( TemporalPointLayer( id, pl ) );
}

PointTimeLayer::TemporalPointLayer PointTimeLayer::getTemporalPointLayer( const unsigned& id )
{	
	assert( id < mTemporalPointLayers.size() );

	return ( mTemporalPointLayers[id] );
}
			
PointTimeLayer::TemporalPointLayers PointTimeLayer::getTemporalPointLayers()
{
	return mTemporalPointLayers;
}

unsigned int PointTimeLayer::maxSize()
{
	unsigned int size		= 0;
	unsigned int maxSize	= 0;

	TemporalPointLayers::iterator itr;
	
	for( itr = mTemporalPointLayers.begin(); itr != mTemporalPointLayers.end(); ++itr )
	{
		size = (*itr).second->size();
		if(  size > maxSize )
		{
			maxSize = size;
		}
	}

	return maxSize;
}

void PointTimeLayer::animate( unsigned int timeStep )
{	
	timeStep %= mTemporalPointLayers.size();

	for( size_t i=0; i < mMaxSize; ++i )	
	{		
		if( i < mTemporalPointLayers[timeStep].second->size() )
		{
			mTemporalTransforms[i]->setNodeMask( 0xff );
			osg::ref_ptr< SimFlow::DataObject::Point > pt = 
				dynamic_cast< SimFlow::DataObject::Point* > ( mTemporalPointLayers[timeStep].second->getDataObject( i ) );
			mTemporalTransforms[i]->setMatrix( osg::Matrix::translate( pt->center() ) );
		}
		else
		{
			mTemporalTransforms[i]->setNodeMask( 0x00 );
		}		
	}	
}

