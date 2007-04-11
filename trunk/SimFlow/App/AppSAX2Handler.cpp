
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#if defined _MSC_VER
  // '__fastcall' incompatible with the '/clr' option: converting to '__stdcall'
  #pragma warning ( disable : 4561 )

  // 'vararg' : causes native code generation for function ...
  #pragma warning ( disable : 4793 )
#endif

#include "AppSAX2Handler.h"

//#include "ObjectCreator.h"

#include "SimFlow/Scene/SceneManager.h"
#include "SimFlow/Layer/Layer.h"
#include "SimFlow/Layer/PointTimeLayer.h"
#include "SimFlow/Layer/PolygonLayer.h"

//#include <Scene/Sequence.h>
//#include <Scene/Model.h>

#include "SimFlow/SimFlowUtil/File/FileOperations.h"

#include "SimFlow/SimFlowUtil/String/StringConv.h"

#include "SimFlow/VTKUtil/VTKReader.h"

#include "SimFlow/Parser/StrX.h"

#include "SimFlow/App/Parser.h"

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
				mGroupType = cxml::StrX( attrs.getValue( attrs.getLocalName( i ) ) ).localForm();
        std::transform( mGroupType.begin(), mGroupType.end(), mGroupType.begin(), tolower );
			}

      if( strcmp( cxml::StrX( attrs.getLocalName( i ) ).localForm(), "id" ) == 0 )
			{
				mId = cxml::StrX( attrs.getValue( attrs.getLocalName( i ) ) ).localForm();
        std::transform( mId.begin(), mId.end(), mId.begin(), tolower );
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

  if( strcmp( cxml::StrX( localname ).localForm(), "lights" ) == 0 )
	{
    mLightsStartTag		=	true;
		mLightsEndTag		  =	false;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "light" ) == 0 )
	{
    mLightStartTag		=	true;
		mLightEndTag		  =	false;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "position" ) == 0 )
	{
    mLightPositionStartTag		=	true;
    mLightPositionEndTag      =	false;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "ambient" ) == 0 )
	{
    mLightAmbientStartTag 		=	true;
    mLightAmbientEndTag       =	false;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "diffuse" ) == 0 )
	{
    mLightDiffuseStartTag 		=	true;
    mLightDiffuseEndTag 		  =	false;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "specular" ) == 0 )
	{
    mLightSpecularStartTag		=	true;
    mLightSpecularEndTag		  =	false;
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
		mGroupEndTag		  =	true;
	}

	if( strcmp( cxml::StrX( localname ).localForm(), "source" ) == 0 )
	{
		mSourceStartTag		=	false;
		mSourceEndTag		  =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "lights" ) == 0 )
	{
    mLightsStartTag		=	false;
		mLightsEndTag		  =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "light" ) == 0 )
	{
    mLightStartTag		=	false;
		mLightEndTag		  =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "position" ) == 0 )
	{
    mLightPositionStartTag		=	false;
    mLightPositionEndTag      =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "ambient" ) == 0 )
	{
    mLightAmbientStartTag 		=	false;
    mLightAmbientEndTag       =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "diffuse" ) == 0 )
	{
    mLightDiffuseStartTag 		=	false;
    mLightDiffuseEndTag 		  =	true;
	}

  if( strcmp( cxml::StrX( localname ).localForm(), "specular" ) == 0 )
	{
    mLightSpecularStartTag		=	false;
    mLightSpecularEndTag		  =	true;
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
				if( mGroupType.compare( "pointtime" ) == 0 )
				{					
					addPointTimeData();
				}
				else if( mGroupType.compare( "polygon" ) == 0 )
				{
          addPolygonData();
				}
				else if( mGroupType.compare( "structeredgrid" )  == 0 )
				{					
         	addStructureGridData();
				}
				else
				{
					std::cout << "[WARNING] Layer type " << mGroupType << " not handled: " << std::endl;
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

/*
void SimFlow::App::AppSAX2Handler::extractData( Scene::Sequence* seq, Scene::Model* model )
{
	osg::ref_ptr< osg::Geode > geode = 
		dynamic_cast< osg::Geode* >( model->getRoot() );	

	if( geode.valid() )
	{
		for( size_t i=0; i < geode->getNumDrawables(); ++i )
		{
			std::cout << "[extractData] Processing drawable: " << i << std::endl;

			osg::ref_ptr< osg::Geometry > drawable	= 
				dynamic_cast< osg::Geometry* >( geode->getDrawable(i) );

			if( drawable.valid() )
			{	
				std::vector< osg::Vec3f > positions;				
				osg::ref_ptr< osg::Vec3Array > points =  dynamic_cast< osg::Vec3Array* >( drawable->getVertexArray() );
				for( size_t j=0; j < points->size(); ++j )
				{
					//std::cout << "[extractData] Processing points: " << j << std::endl;								
					positions.push_back( ( *points.get() )[j] );							
				}								
				seq->addPositions( positions );
			}
		}
	}
}
*/

void SimFlow::App::AppSAX2Handler::addPointTimeData()
{
  osg::ref_ptr< SimFlow::Layer::PointTimeLayer > pTimeLayer( new SimFlow::Layer::PointTimeLayer() );
					
	pTimeLayer->layerId( mId );
	pTimeLayer->renderType( mPrimitiveType );
	pTimeLayer->renderSize( mPrimitiveSize );

	mSceneManager->addLayer( pTimeLayer.get() );								

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

void SimFlow::App::AppSAX2Handler::addPolygonData()
{
  osg::ref_ptr< SimFlow::Layer::PolygonLayer > pLayer( new SimFlow::Layer::PolygonLayer() );
	pLayer->layerId( mId );
	mSceneManager->addLayer( pLayer.get() );
  addDataAsPolygonData( pLayer.get(), DataObject::DataObject::POLYGONDATA );
}

void SimFlow::App::AppSAX2Handler::addStructureGridData()
{
  osg::ref_ptr< SimFlow::Layer::PolygonLayer > pLayer( new SimFlow::Layer::PolygonLayer() );
	pLayer->layerId( mId );
	mSceneManager->addLayer( pLayer.get() );
  addDataAsPolygonData( pLayer.get(), DataObject::DataObject::STRUCTUREDGRID ); 
}

void SimFlow::App::AppSAX2Handler::addDataAsPolygonData( SimFlow::Layer::PolygonLayer* pLayer, unsigned int type )
{
  switch( type )
  {
  case SimFlow::DataObject::DataObject::STRUCTUREDGRID:
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
    case SimFlow::DataObject::DataObject::POLYGONDATA:
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
      break;
    }
  };	
}
