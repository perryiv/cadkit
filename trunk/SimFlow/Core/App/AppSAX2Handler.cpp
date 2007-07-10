
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#if defined _MSC_VER

// '__fastcall' incompatible with the '/clr' option: converting to '__stdcall'
#pragma warning ( disable : 4561 )

// 'vararg' : causes native code generation for function ...
#pragma warning ( disable : 4793 )

#endif

#include "AppSAX2Handler.h"

#include "SimFlow/Core/Scene/SceneManager.h"
#include "SimFlow/Core/Layer/Layer.h"
#include "SimFlow/Core/Layer/PointTimeLayer.h"
#include "SimFlow/Core/Layer/PolygonLayer.h"
#include "SimFlow/Core/SimFlowUtil/FileOperations.h"
#include "SimFlow/Core/SimFlowUtil/StringConv.h"
#include "SimFlow/Core/Parser/StrX.h"
#include "SimFlow/Core/App/Parser.h"

#include "SimFlow/VTKUtil/VTKReader.h"

#include "osg/Geometry"

#include "osgDB/ReadFile"
#include "osgDB/FileNameUtils"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <locale>

namespace cxml = SimFlow::XMLParser;

void SimFlow::App::AppSAX2Handler::startElement( const XMLCh *const uri, 
												 const XMLCh *const localname, 
												 const XMLCh *const qname, 
												 const xercesc_2_7::Attributes &attrs )
{
	fElementCount++;
	fAttrCount++;

	//std::cout << "Tag found: " << ( cxml::StrX( localname ).localForm() ) << std::endl;

	if( strcmp( ( cxml::StrX( localname ) ).localForm(), "scene" ) == 0 )
	{
		mSceneStartTag		=	true;
		mSceneEndTag		  =	false;
	}

	if( strcmp( cxml::StrX( localname ).localForm(), "group" ) == 0 )
	{
		for( size_t i=0; i < attrs.getLength(); ++i )
		{
			if( strcmp( cxml::StrX( attrs.getLocalName( i ) ).localForm(), "type" ) == 0 )
			{
				mLayerType = cxml::StrX( attrs.getValue( attrs.getLocalName( i ) ) ).localForm();
        			//std::transform( mLayerType.begin(), mLayerType.end(), mLayerType.begin(), tolower );
			}

			if( strcmp( cxml::StrX( attrs.getLocalName( i ) ).localForm(), "primitive" ) == 0 )
			{
				mPrimitiveType = SimFlow::SimFlowUtil::String::convert( mPrimitiveType, cxml::StrX( attrs.getValue( attrs.getLocalName( i ) ) ).localForm() );
			}

			if( strcmp( cxml::StrX( attrs.getLocalName( i ) ).localForm(), "size" ) == 0 )
			{
				mPrimitiveSize = SimFlow::SimFlowUtil::String::convert( mPrimitiveSize, cxml::StrX( attrs.getValue( attrs.getLocalName( i ) ) ).localForm() );
			}
		}

		mGroupStartTag		=	true;
		mGroupEndTag		  =	false;
	}

	if( strcmp( cxml::StrX( localname ).localForm(), "source" ) == 0 )
	{
		mSourceStartTag		=	true;
		mSourceEndTag		  =	false;
	}

	// Implement later. 
	/*
	if( strcmp( cxml::StrX( localname ).localForm(), "model" ) == 0 )
	{
		mStartModelTag		=	true;
		mEndModelTag		=	false;
	}
	*/
}

void SimFlow::App::AppSAX2Handler::endElement( const XMLCh *const uri, 
											                         const XMLCh *const localname, 
											                         const XMLCh *const qname )
{
	if( strcmp( cxml::StrX( localname ).localForm(), "scene" ) == 0 )
	{
		mSceneStartTag		=	false;
		mSceneEndTag   		=	true;
	}

	if( strcmp( cxml::StrX( localname ).localForm(), "group" ) == 0 )
	{
		mGroupStartTag		=	false;
		mGroupEndTag		=	true;
	}

	if( strcmp( cxml::StrX( localname ).localForm(), "source" ) == 0 )
	{
		mSourceStartTag		=	false;
		mSourceEndTag		=	true;
	}

	// Implement it later. 
	/*
	if( strcmp( cxml::StrX( localname ).localForm(), "model" ) == 0 )
	{
		mStartModelTag		=	false;
		mEndModelTag		=	true;
	}
	*/
}

void SimFlow::App::AppSAX2Handler::characters( const XMLCh *const chars, 
											                         const unsigned int length )
{
	fCharacterCount += length;

	if( mSceneStartTag )
	{					
		if( mGroupStartTag )
		{			
			mGroupStartTag = false;
		}

		/*
		if( mStartModelTag && !mGroupEndTag )
		{
			std::string file = cxml::StrX( chars ).localForm();
			osg::ref_ptr< Scene::Model > model = newModel( file );			
			mSceneManager->getLastScene()->getLastLayer()->addModel( model.get() );
			mStartModelTag = false;
		}
		*/	
	
		if( mSourceStartTag && !mGroupEndTag )
		{
			// Assuming here that we are only dealing with PointTimeLayer. 
			mFileNames.clear();			
			std::string dirPath = std::string( cxml::StrX( chars ).localForm() );

      if( !SimFlowUtil::File::exists( dirPath ) ) 
      {
        std::cerr << "Error:ErrorNo Path " << dirPath << " not found: " << std::endl;
        return;
      }
				
			SimFlowUtil::File::findFile( dirPath, "", mFileNames );      
			
			if( !mFileNames.empty() )
			{
				if( mLayerType.compare( "pointtime" ) == 0 )
				{
					osg::ref_ptr< SimFlow::Layer::PointTimeLayer > pTimeLayer( new SimFlow::Layer::PointTimeLayer() );
					
					pTimeLayer->layerID( mSceneManager->numberOfLayers() );
					pTimeLayer->renderType( mPrimitiveType );
					pTimeLayer->renderSize( mPrimitiveSize );

					mSceneManager->addLayer( pTimeLayer.get() );								
					addPointTimeData( pTimeLayer.get() );								
				}
				else if( mLayerType.compare( "polygon" ) == 0 )
				{
					osg::ref_ptr< SimFlow::Layer::PolygonLayer > pLayer( new SimFlow::Layer::PolygonLayer() );
					pLayer->layerID( mSceneManager->numberOfLayers() );
					mSceneManager->addLayer( pLayer.get() );
          addDataAsPolygonData( pLayer.get(), SimFlow::VTKUtil::VTKReader::POLYGONALDATA );
				}
				else if( mLayerType.compare( "structeredgrid" )  == 0 )
				{
					osg::ref_ptr< SimFlow::Layer::PolygonLayer > pLayer( new SimFlow::Layer::PolygonLayer() );
					pLayer->layerID( mSceneManager->numberOfLayers() );
					mSceneManager->addLayer( pLayer.get() );
          addDataAsPolygonData( pLayer.get(), SimFlow::VTKUtil::VTKReader::STRUCTUREDGRID );
				}
				else
				{
					std::cout << "[WARNING] Layer type " << mLayerType << " not handled: " << std::endl;
				}
			}
			mSourceStartTag = false;
		}

    // Clear the list of files. 
    mFileNames.clear();

		/*
		if( mStartModelTag && !mEndSequenceTag )
		{			
			std::string file = cxml::StrX( chars ).localForm();
			osg::ref_ptr< Scene::Model > model = newModel( file );
			osg::ref_ptr< Scene::Sequence > seq = mSceneManager->getLastScene()->getLastSequence();
			extractData( seq.get(), model.get() );
			mStartModelTag = false;
		}

		if( mStartDirTag && !mEndSequenceTag )
		{
			// Attributes for the sequence. ( check whether this sequence is point data set 
			// or surface data set. 
			mFileNames.clear();			
			boost::filesystem::path dir( std::string( cxml::StrX( chars ).localForm() ), fs::native );			
			file::findFile(  dir,  "vtk", mFileNames ); 

			osg::ref_ptr< Scene::Sequence > seq = mSceneManager->getLastScene()->getLastSequence();

			for( size_t i=0; i < mFileNames.size(); ++i )
			{			
				osg::ref_ptr< Scene::Model > model = newModel( mFileNames[i] );
				extractData( seq.get(), model.get() );
			}
			mStartDirTag = false;
		}
		*/
	}
}


void SimFlow::App::AppSAX2Handler::addPointTimeData( SimFlow::Layer::PointTimeLayer* pTimeLayer )
{
  const FileNames::size_type numFiles ( mFileNames.size() );

	for( size_t i = 0; i < numFiles; ++i )
	{
		std::cout << "Processing time step: " << i << '\r' << std::flush;

		osg::ref_ptr< SimFlow::Layer::PointLayer > pLayer( new SimFlow::Layer::PointLayer() );

		osg::ref_ptr< osg::Geode > geode;

		if( osgDB::getFileExtension( mFileNames[i] ).compare( "vtk" ) == 0 )
		{
			geode = dynamic_cast< osg::Geode* >( SimFlow::VTKUtil::VTKReader::readVTKFile( mFileNames[i] ) );		
		}
		else
		{
			geode = dynamic_cast< osg::Geode* >( osgDB::readNodeFile( mFileNames[i] ) );
		}

		if( geode.valid() )
		{
			for( size_t j = 0; j < geode->getNumDrawables(); ++j )
			{
				osg::ref_ptr< osg::Geometry > drawable	= dynamic_cast< osg::Geometry* >( geode->getDrawable( j ) );

				if( drawable.valid() )
				{										
					osg::ref_ptr< osg::Vec3Array > pPositions =  dynamic_cast< osg::Vec3Array* >( drawable->getVertexArray() );
					for( size_t k = 0; k < pPositions->size(); ++k )
					{
						osg::ref_ptr< SimFlow::DataObject::Point > point( new SimFlow::DataObject::Point() );
						point->center( ( *pPositions.get() )[k] );							
						pLayer->addDataObject( point.get() );
					}
				}
			}		
		}
		pTimeLayer->addTemporalPointLayer( i, pLayer.get() );		
	}
}

void SimFlow::App::AppSAX2Handler::addDataAsPolygonData( SimFlow::Layer::PolygonLayer* pLayer, unsigned int type )
{
  switch( type )
  {
    case SimFlow::VTKUtil::VTKReader::STRUCTUREDGRID:
    {
      for( size_t i=0; i < mFileNames.size(); ++i )
	    {
		    std::cout << "Reading file: " << mFileNames[i] << "\r" << std::flush;
		    osg::ref_ptr< SimFlow::DataObject::Polygon > polyData( new SimFlow::DataObject::Polygon() );
		    if( osgDB::getFileExtension( mFileNames[i] ).compare( "vtk" ) == 0 )
		    {		
			    polyData->polyData( SimFlow::VTKUtil::VTKReader::readVTKFile( mFileNames[i], type ) );
			    pLayer->addDataObject( polyData.get() );
		    }	
	    }
      break;
    }
    case SimFlow::VTKUtil::VTKReader::POLYGONALDATA:
    {
      for( size_t i=0; i < mFileNames.size(); ++i )
	    {
        std::cout << "Reading file: " << mFileNames[i] << "\r" << std::flush;
        osg::ref_ptr< SimFlow::DataObject::Polygon > polyData( new SimFlow::DataObject::Polygon() );
        osg::ref_ptr< osg::Node > node = osgDB::readNodeFile( mFileNames[i] );
        if( node.valid() )
        {
          polyData->polyData( node.get() );
          pLayer->addDataObject( polyData.get() );
        }
      }
    }
  };	
}

