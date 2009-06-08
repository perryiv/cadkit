
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

// #include "Experimental/DRTSim/DRTSim/DrtSimReader.h"

#include "DRTSimReader.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"

#include "Usul/CommandLine/Arguments.h"
#include "Usul/System/Host.h"
#include "Usul/Scope/CurrentDirectory.h"
#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"

#include "osgDB/ReadFile"
#include "osg/Group"
#include "osg/Switch"
#include "osg/MatrixTransform"
#include "osgDB/WriteFile"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/StateSet"
#include "osg/Texture2D"
#include "osg/BoundingBox"

#include <iterator>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>


//USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DrtSimReader, DrtSimReader::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

DrtSimReader::DrtSimReader() : 
  _root( new osg::Group ),
  _counter( 0 ),
  _workingDir( "" )
{
  USUL_TRACE_SCOPE;

  // added by Wei CHEN

  _winHeight		= 1024;
  _winWidth			= 1280;
  _winMargin		= 50;

  _areaFileName		= "AreaDetails.dat";
  _agentFileName	= "AgentDetails.dat";
  _stockFileName	= "StockDetails.dat";
  _transFileName	= "Transhipments.dat";
  _sporeFileName	= "SporeDetails.dat";
  _legendMachine  = "localhost";
  _writerMachine  = "localhost";

  // end here ( added by W.C )

  _mpdWriter = new ModelPresentationLib();
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DrtSimReader::~DrtSimReader()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::write ( const std::string &name, Unknown *caller, Unknown *progress  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );

  _drtFileName = Usul::File::base( name );
  this->_read( name, caller, progress );

  this->_init ( progress );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::_read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  

  //set the working directory
  _workingDir = Usul::File::directory( filename, true );
  _area.setWorkingDir ( _workingDir );
  _agent.setWorkingDir( _workingDir );
  _stock.setWorkingDir( _workingDir );
  _trans.setWorkingDir( _workingDir );

  Usul::Scope::CurrentDirectory cwd ( _workingDir );

  std::ifstream infile( filename.c_str(), std::ios::in );
  if( !infile ) 
  {
    throw std::runtime_error ( Usul::Strings::format ( "Error 8651628270: unable to open the simulation file: ", filename ) );
  }
  
  while( !infile.eof() )
  {
    std::string id, fn;
    infile >> id >> fn;
	  //std::getline( infile, id, ':');		// agent: AgentDetails.dat
	  //std::getline( infile, fn, '\n');
	  //fn.erase(0, 1);		// erase the space
	  //std::cout << Usul::Strings::format( "ID=",id,"\tfilename=",fn ) << std::endl;
	  if( "agent" == id )		_agentFileName = fn;
	  if( "area"  == id )		_areaFileName  = fn;
	  if( "stock" == id )		_stockFileName = fn;
	  if( "spore" == id )		_sporeFileName = fn;
	  if( "trans" == id )		_transFileName = fn;
    if( "legend" == id )  _legendMachine = fn;
    if( "writer" == id )  _writerMachine = fn;

          
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::_init ( Unknown *progress )
{
  USUL_TRACE_SCOPE;
  
  
 // add code here (application) by Wei.CHEN
	
  if (	!_readAreaDetailsFile( ) 	|| 
		    !_readAgentDetailsFile( )	||
		    !_readStockDetailsFile( )	|| 
		  /*!_readSporeDetailsFile( )	||*/ 
		    !_readTranshipmentsFile( )		)
	{
    throw std::runtime_error ( "Error 2797071041: Couldn't open one or more files needed to run the simulation" );
	}

  _root->removeChild( 0, _root->getNumChildren() );

  _agent.setWriter( _mpdWriter.get() );
  _stock.setWriter( _mpdWriter.get() );
  _trans.setWriter( _mpdWriter.get() );

  _mpdWriter->addTimeSet( "Steps", "Variables", 0 );
  _mpdWriter->addTimeSet( "Agents", "Variables", 0 );
  _mpdWriter->addTimeSet( "Stock", "Variables", 0 );
  _mpdWriter->addTimeSet( "Transhipments", "Variables", 0 );

  // put "current step" and Agent & Stock color bars
  {
		osg::ref_ptr< osg::Camera > camera ( new osg::Camera );

		camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
        camera->setProjectionMatrixAsOrtho2D( 0, 1280, 0, 1024 );
        camera->setViewMatrix( osg::Matrix::identity( ) );
		camera->setClearMask( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ); // GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT
		// camera->setClearColor( osg::Vec4( 0.3f, 0.3f, 0.3f, 1.0f ) );
        if( "localhost" == _legendMachine || 
            Usul::System::Host::name() == _legendMachine )
        {
          camera->addChild( _createHUDText() );
        }
        camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

		//_root->addChild( camera.get() );

  }

	// area
#if 1
  osg::ref_ptr< osg::Group > areaNode ( new osg::Group );
  _area.setWriter( _mpdWriter.get() );
  areaNode->addChild( _area.buildAreaScene( ) );
	//_root->addChild( areaNode.get() );
#endif

	// agent
#if 1
  osg::ref_ptr< osg::Group > agentNode ( new osg::Group );
  
  
  agentNode->addChild( _agent.buildAgentScene( ) );
  //_mpdWriter->addDynamicSet( "Steps", "Variables", "step", "osg", _workingDir, _agent.getTotalSteps() - 1 );
  //_mpdWriter->addDynamicSet( "Agents", "Variables", "agent", "osg", _workingDir, _agent.getTotalSteps() - 1 );
  //_root->addChild( agentNode.get() );
#endif

	// stock
#if 1
  osg::ref_ptr< osg::Group > stockNode ( new osg::Group );
  
  stockNode->addChild( _stock.buildStockScene( ) );
  //_mpdWriter->addDynamicSet( "Stock", "Variables", "stock", "osg", _workingDir, _stock.getTotalSteps() - 1 );
	//_root->addChild( stockNode.get() );
#endif

	// transhipment
#if 1
  osg::ref_ptr< osg::Group > transNode ( new osg::Group );
  transNode->addChild( _trans.buildTranshipmentScene( ) );
	//_root->addChild( transNode.get() );
#endif

	// spore
#if 0
  osg::ref_ptr< osg::Group > sporeNode ( new osg::Group );
  sporeNode->addChild( _spore.buildSporeScene( ) );
	_root->addChild( sporeNode.get() );
#endif

  // end here ( added by W.C )

  

  //---------------------------------------------------
  // create and open a model presentation document here
  //---------------------------------------------------
  _mpdWriter->buildXMLString();
  std::string filename ( _workingDir + _drtFileName + ".mpd" );

  _mpdWriter->write( filename );
   

  //---------------------------------------------------

  //std::cout << "Buildscene complete!" << std::endl;
  //return _root.get();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
  //std::cout << "Done" << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
// Added by Wei.CHEN
//
///////////////////////////////////////////////////////////////////////////////

bool		DrtSimReader::_readAreaDetailsFile( )
{

	// std::string inputBuffer = "AreaDetails.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _area.setAreaFileName ( _areaFileName ) )		
	{
		// for stock information
		_stock.setAreaSizeY( _area.getAreaSizeY( ) );
		_stock.setAreaSizeX( _area.getAreaSizeX( ) );
		_stock.setNumOfHospitals( _area.getNumOfHospitals( ) );
		_stock.setHospitalReserve( );

		// for agent information
		_agent.setAreaSizeX( _area.getAreaSizeX( ) );
		_agent.setAreaSizeY( _area.getAreaSizeY( ) );
		
		// for transhipments
		_trans.setAreaSizeX( _area.getAreaSizeX( ) );
		_trans.setAreaSizeY( _area.getAreaSizeY( ) );
		_trans.setNumOfHospitals( _area.getNumOfHospitals( ) );
		_trans.setHospitalReserve( );

		for(unsigned int i = 0; i < _area.getNumOfHospitals(); ++i)
		{
			osg::Vec3 p ( _area.getHospitalCoordinates( i )  );
			_stock.setHospitalCoordinates( p );
			_trans.setHospitalCoordinates( p );
		}
		
		return true;
	}

	return false;


}


bool		DrtSimReader::_readAgentDetailsFile( )
{

	// std::string inputBuffer = "AgentDetails.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _agent.setAgentFileName ( _agentFileName ) )		
	{
		_trans.setNumOfSteps( _agent.getNumOfAgentSteps( ) );
		_spore.setNumofSteps( _agent.getNumOfAgentSteps( ) );

		return true;
	}

	return false;

}



bool		DrtSimReader::_readSporeDetailsFile( )
{

	// std::string inputBuffer = "SporeDetails.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _spore.setSporeFileName ( _sporeFileName ) )
	{
		// _spore.setNumofSteps( _agent.getNumOfAgentSteps( ) );
		//	_spore.setNumofSteps( _stock.getNumOfStockSteps( ) );		// should be a same result

		_spore.setAreaSizeX( _area.getAreaSizeX( ) );
		_spore.setAreaSizeY( _area.getAreaSizeY( ) );
		
		return true;
	}

	return false;

}



bool		DrtSimReader::_readStockDetailsFile( )
{

	// std::string inputBuffer = "StockDetails.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _stock.setStockFileName ( _stockFileName ) )		return true;

	return false;

}



bool		DrtSimReader::_readTranshipmentsFile( )
{

	// std::string inputBuffer = "Transhipments.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _trans.setTranshipmentFileName ( _transFileName ) )		
	{
		_stock.setNumofTranshipmentSteps( _trans.getNumOfTranshipments( ) );
		_stock.setNumofTranshipmentMovements( _trans.getNumOfTranMovements( ) );
		_stock.setTranshipmentReserve( );

		_agent.setNumofTranshipmentSteps( _trans.getNumOfTranshipments( ) );
		_agent.setNumofTranshipmentMovements( _trans.getNumOfTranMovements( ) );
		_agent.setTranshipmentReserve( );

		_spore.setNumofTranshipmentSteps( _trans.getNumOfTranshipments( ) );
		_spore.setNumofTranshipmentMovements( _trans.getNumOfTranMovements( ) );
		_spore.setTranshipmentReserve( );

		for( unsigned int i = 0; i < _trans.getNumOfTranshipments(); ++i )
		{
			_stock.setTranshipmentSteps( _trans.getStepOfTranshipments( i ) );
			_agent.setTranshipmentSteps( _trans.getStepOfTranshipments( i ) );
			_spore.setTranshipmentSteps( _trans.getStepOfTranshipments( i ) );
		}

		return true;
	}

	return false;


}


osg::Group*		DrtSimReader::_createHUDText( )
{

	_agent.setWindowHeight	( _winHeight );
	_agent.setWindowWidth	( _winWidth );
	_agent.setWindowMargin	( _winMargin );

	_stock.setWindowHeight	( _winHeight );
	_stock.setWindowWidth	( _winWidth  );
	_stock.setWindowMargin	( _winMargin );

	osg::ref_ptr< osg::Group > group ( new osg::Group );

	group->addChild( _agent.createAgentColorCodeBar( ) );
	group->addChild( _stock.createStockColorCodeBar( ) );
	

	// to display "Current Step = ", only need one sequence from either Agent or Stock
	
   std::string path = _workingDir;
#if 1
    {
      osg::ref_ptr< osg::Camera > camera ( new osg::Camera );

      camera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
      camera->setProjectionMatrixAsOrtho2D( 0, 1280, 0, 1024 );
      camera->setViewMatrix( osg::Matrix::identity( ) );
      camera->setClearMask( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
      camera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
      camera->addChild( group.get() );
      std::string filename = Usul::Strings::format( path, "/legend", _counter, ".osg" );
      osgDB::writeNodeFile( *( camera.get() ), filename.c_str() );
      _mpdWriter->addModel( "legend", filename );
      _mpdWriter->addModel( "hide", "" );
      _mpdWriter->addSet( "Legend", "Settings" );
      _mpdWriter->addModelToSet( "hide", "Legend", "Hide" );
      _mpdWriter->addModelToSet( "legend", "Legend", "Show" );
      

    }
#endif
  group->addChild( _agent.createAgentStepSequence( ) );
    
	return group.release();

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void DrtSimReader::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );
  if ( 0x0 == document )
    return;
#if 1
  if( true == document->canOpen( file ) )
    document->open ( file, caller, progress );
  else
    std::cout << "Unable to open document for file: " << file << std::endl;
#else
    document->open ( file, caller );
#endif
    //std::cout << "Done loading file: " << file << std::endl;
  
	
}
