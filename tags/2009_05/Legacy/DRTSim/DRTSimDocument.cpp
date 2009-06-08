
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

// #include "Experimental/DRTSim/DRTSim/DRTSimDocument.h"

#include "DRTSimDocument.h"
#include "DrtAnimation.h"
#include "DrtAgentShow.h"
#include "DrtStockShow.h"
#include "DrtSporeShow.h"
#include "DrtTransShow.h"
#include "DrtStartOver.h"

#include "Usul/Interfaces/IDisplaylists.h"
#include "Usul/Interfaces/IViewMatrix.h"
//#include "Usul/Interfaces/IViewPort.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/CommandLine/Arguments.h"
#include "Usul/Predicates/FileExists.h"
#include "Usul/System/Host.h"

#include "Usul/Strings/Convert.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Trace/Trace.h"
#include "Usul/File/Path.h"
#include "Usul/File/Find.h"
#include "Usul/File/Slash.h"
#include "Usul/Print/Matrix.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Math/Functions.h"
#include "Usul/Math/MinMax.h"

#include "OsgTools/DisplayLists.h"
#include "OsgTools/Group.h"
#include "OsgTools/Convert.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"
#include "MenuKit/ToggleButton.h"
#include "MenuKit/RadioButton.h"

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



USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( DRTSimDocument, DRTSimDocument::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::DRTSimDocument() : 
  BaseClass ( "DRT Simulation Document" ),
  _root( new osg::Group ),
  _isAnimating( true ),
  _isAgentOn( true ),
  _isSporeOn( true ),
  _isStockOn( true ),
  _isTransOn( true ),
  _counter( 0 )
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
  _legendMachine        = "localhost";

  // end here ( added by W.C )
   
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::~DRTSimDocument()
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear the document.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::clear ( Unknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query the interfaces
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *DRTSimDocument::queryInterface ( unsigned long iid )
{
  USUL_TRACE_SCOPE;

  switch ( iid )
  {
  case Usul::Interfaces::IBuildScene::IID:
    return static_cast < Usul::Interfaces::IBuildScene* > ( this );
  case Usul::Interfaces::IUpdateListener::IID:
    return static_cast < Usul::Interfaces::IUpdateListener* > ( this );
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd* > ( this );
  case Usul::Interfaces::IDrtCommands::IID:
    return static_cast < Usul::Interfaces::IDrtCommands* > ( this );
  default:
    return BaseClass::queryInterface ( iid );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document exports given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DRTSimDocument::canExport ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document inserts given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DRTSimDocument::canInsert ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  return this->canOpen ( file );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document reads given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DRTSimDocument::canOpen ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "drt" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return true if this document writes given extension
//
///////////////////////////////////////////////////////////////////////////////

bool DRTSimDocument::canSave ( const std::string &file ) const
{
  USUL_TRACE_SCOPE;
  const std::string ext ( Usul::Strings::lowerCase ( Usul::File::extension ( file ) ) );
  return ( ext == "drt" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the document to given file name.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::write ( const std::string &name, Unknown *caller, Unknown *progress  ) const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::read ( const std::string &name, Unknown *caller, Unknown *progress )
{
  USUL_TRACE_SCOPE;
  //Guard guard ( this->mutex() );

  // added by Wei.CHEN
  this->_read( name, caller, progress );

  // name == santanam.drt ??
  

  // end here ( added by W.C )

}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the document.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::_read ( const std::string &filename, Unknown *caller, Unknown *progress )
{
  std::string id, fn;

  std::ifstream infile( filename.c_str(), std::ios::in );
  if( !infile ) {
	  std::cerr << "Error: unable to open drt file!\n";
	  ///exit( 0 );
    return;
  }

  while( !infile.eof() )
  {
	  std::getline( infile, id, ':');		// agent: AgentDetails.dat
	  std::getline( infile, fn, '\n');
	  fn.erase(0, 1);		// erase the space
	  std::cout << Usul::Strings::format( "ID=",id,"\tfilename=",fn ) << std::endl;
	  if( "agent" == id )		_agentFileName = fn;
	  if( "area"  == id )		_areaFileName  = fn;
	  if( "stock" == id )		_stockFileName = fn;
	  if( "spore" == id )		_sporeFileName = fn;
	  if( "trans" == id )		_transFileName = fn;
          if( "legend" == id )          _legendMachine = fn;

          
  }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can save.
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::Filters DRTSimDocument::filtersSave() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "DRT Simulation (*.drt)", "*.drt" ) );   // filter
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can export.
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::Filters DRTSimDocument::filtersExport() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can open.
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::Filters DRTSimDocument::filtersOpen() const
{
  USUL_TRACE_SCOPE;
  Filters filters;
  filters.push_back ( Filter ( "DRT Simulation (*.drt)", "*.drt" ) );
  return filters;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the filters that correspond to what this document can insert.
//
///////////////////////////////////////////////////////////////////////////////

DRTSimDocument::Filters DRTSimDocument::filtersInsert() const
{
  USUL_TRACE_SCOPE;
  return this->filtersOpen();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build the scene.
//
///////////////////////////////////////////////////////////////////////////////

osg::Node *DRTSimDocument::buildScene ( const BaseClass::Options &options, Unknown *caller )
{
  USUL_TRACE_SCOPE;
  _root->removeChild( 0, _root->getNumChildren() );
  
 // add code here (application) by Wei.CHEN
	
  if (	!_readAreaDetailsFile( ) 	|| 
		!_readAgentDetailsFile( )	||
		!_readStockDetailsFile( )	|| 
		!_readSporeDetailsFile( )	|| 
		!_readTranshipmentsFile( )		)
	{
		std::cerr << " Can not open files, Exit !!\n";
		exit( 0 );		// program got exit once any one of files failed to open
	}

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

		_root->addChild( camera.get() );

  }

	// area
#if 1
  osg::ref_ptr< osg::Group > areaNode ( new osg::Group );
  areaNode->addChild( _area.buildAreaScene( ) );
	_root->addChild( areaNode.get() );
#endif
	// agent
#if 1
  osg::ref_ptr< osg::Group > agentNode ( new osg::Group );
  agentNode->addChild( _agent.buildAgentScene( ) );
  _root->addChild( agentNode.get() );
#endif
	// stock
#if 1
  osg::ref_ptr< osg::Group > stockNode ( new osg::Group );
  stockNode->addChild( _stock.buildStockScene( ) );
	_root->addChild( stockNode.get() );
#endif
	// transhipment
#if 1
  osg::ref_ptr< osg::Group > transNode ( new osg::Group );
  transNode->addChild( _trans.buildTranshipmentScene( ) );
	_root->addChild( transNode.get() );
#endif
	// spore
#if 0
  osg::ref_ptr< osg::Group > sporeNode ( new osg::Group );
  sporeNode->addChild( _spore.buildSporeScene( ) );
	_root->addChild( sporeNode.get() );
#endif

  // end here ( added by W.C )

  // make osg files from each node.
  std::string path = "C:/data/Santanam/output/";
#if 1 // area
  {
    std::string filename = Usul::Strings::format( path, "area", ".osg" );
    osgDB::writeNodeFile( *( areaNode.get() ), filename.c_str() );
  }
#endif
#if 0 // agent
  {
    std::string filename = Usul::Strings::format( path, "agent", ".osg" );
    osgDB::writeNodeFile( *( agentNode.get() ), filename.c_str() );
  }
#endif
#if 0 // transhipment
  {
    std::string filename = Usul::Strings::format( path, "trans", ".osg" );
    osgDB::writeNodeFile( *( transNode.get() ), filename.c_str() );
  }
#endif
#if 0 // stock
  {
    std::string filename = Usul::Strings::format( path, "stock", ".osg" );
    osgDB::writeNodeFile( *( stockNode.get() ), filename.c_str() );
  }
#endif
#if 0 // spore
  {
    std::string filename = Usul::Strings::format( path, "spore", _counter, ".osg" );
    osgDB::writeNodeFile( *( sporeNode.get() ), filename.c_str() );
  }
#endif


  std::cout << "Buildscene complete!" << std::endl;
  return _root.get();
}



///////////////////////////////////////////////////////////////////////////////
//
//  Update.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::updateNotify ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

}


///////////////////////////////////////////////////////////////////////////////
//
//  Open the document.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::_openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller )
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
  std::cout << "Done" << std::endl;
  
	
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convenience function to set status bar and flush events.
//
///////////////////////////////////////////////////////////////////////////////

void DRTSimDocument::_setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this->mutex() );
  Usul::Interfaces::IStatusBar::QueryPtr status ( caller );
  if ( status.valid() )
    status->setStatusBarText ( text, true );
}


///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

//void DRTSimDocument::_readVertices( const std::string& filename )
//{
  //const unsigned long int bufSize ( 4095 );
  //char buffer[bufSize+1];
  //std::ifstream in;
  //in.rdbuf()->pubsetbuf ( buffer, bufSize );
  //in.open ( filename.c_str() );
  //if ( !in.is_open() )
  //  throw std::runtime_error ( "Error 1099205557: Failed to open file: " + filename );

  //
  //in >> _width >> _height >> _depth;
  //
  //_vertices.reserve( _width * _height * _depth );

  //std::cout << "Reading Vertices..." << std::endl;
  //for( unsigned int x = 0; x < _width; ++x )
  //{
  //  for( unsigned int y = 0; y < _height; ++y )
  //  {
  //    for( unsigned int z = 0; z < _depth; ++z )
  //    {
  //      unsigned int v;
  //      in >> v;
  //      _vertices.push_back( static_cast< unsigned char > ( v ) );
  //      
  //    }
  //  }
  //  std::cout << "\rPercent complete: " << static_cast< float >( x ) * 100.0f / static_cast< float > ( _width );
  //}
  //std::cout << "\rPercent complete: 100               " << std::endl;
//}



void DRTSimDocument::menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown *caller )
{
  typedef MenuKit::ToggleButton ToggleButton;
  typedef MenuKit::Button       Button;
  typedef MenuKit::RadioButton  Radio;

  Usul::Interfaces::IUnknown::QueryPtr me ( this );

  //if( true == _showTools )
  //{
  //  MenuKit::Menu::RefPtr toolsMenu ( new MenuKit::Menu ( "Tools", MenuKit::Menu::VERTICAL ) );
  //  toolsMenu->append ( new Button ( new MpdTools( me.get(), "ShowMatrix" ) ) );
  //  menu.append( toolsMenu );
  //}

  MenuKit::Menu::RefPtr commands ( new MenuKit::Menu ( "Commands", MenuKit::Menu::VERTICAL ) );
  commands->append( new ToggleButton ( new DrtAnimation( me.get() ) ) );
  commands->append( new Button ( new DrtStartOver( me.get() ) ) );
  commands->append( new ToggleButton ( new DrtAgentShow( me.get() ) ) );
  commands->append( new ToggleButton ( new DrtSporeShow( me.get() ) ) );
  commands->append( new ToggleButton ( new DrtStockShow( me.get() ) ) );
  commands->append( new ToggleButton ( new DrtTransShow( me.get() ) ) );
  menu.append( commands.get() );

}



///////////////////////////////////////////////////////////////////////////////
//
// Added by Wei.CHEN
//
///////////////////////////////////////////////////////////////////////////////

bool		DRTSimDocument::_readAreaDetailsFile( )
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


bool		DRTSimDocument::_readAgentDetailsFile( )
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



bool		DRTSimDocument::_readSporeDetailsFile( )
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



bool		DRTSimDocument::_readStockDetailsFile( )
{

	// std::string inputBuffer = "StockDetails.dat";
	// std::transform( inputBuffer.begin(), inputBuffer.end(), inputBuffer.begin(), ::tolower);  // change to low case

	if ( _stock.setStockFileName ( _stockFileName ) )		return true;

	return false;

}



bool		DRTSimDocument::_readTranshipmentsFile( )
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


osg::Group*		DRTSimDocument::_createHUDText( )
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
	
   std::string path = "C:/data/Santanam/output/";
#if 1
    {
      std::string filename = Usul::Strings::format( path, "legend", _counter, ".osg" );
      osgDB::writeNodeFile( *( group.get() ), filename.c_str() );
    }
#endif
  group->addChild( _agent.createAgentStepSequence( ) );
    
	return group.release();

}



///////////////////////////////////////////////////////////////////////////////
//
// set the animation state
//
///////////////////////////////////////////////////////////////////////////////


void		DRTSimDocument::animating ( bool state )
{
	Guard guard ( this->mutex() );
	_isAnimating = state;

	// Put animation toggle implementation here:
	_agent.resume ();
	_stock.resume ();
	_trans.resume ();
	_spore.resume ();

}

// get the animation state
bool		DRTSimDocument::animating ()
{
  Guard guard ( this->mutex() );
  return _isAnimating;
}



void		DRTSimDocument::startOver ()
{
	Guard guard ( this->mutex() );
	//_isStartOver = state;

	// Put animation toggle implementation here:
	_agent.startOver ();
	_stock.startOver ();
	_trans.startOver ();
	_spore.startOver ();

}

// get the animation state
//bool		DRTSimDocument::startOver ()
//{
//  Guard guard ( this->mutex() );
//  return _isStartOver;
//}


void		DRTSimDocument::agentShow ( bool state )
{
	Guard guard ( this->mutex() );
	_isAgentOn = state;

	// put animation toggle implementation here
	if ( _agent.shown () )
		_agent.hide();
	else
		_agent.show();

}

bool		DRTSimDocument::agentShow ( )
{
	Guard guard ( this->mutex() );
	return _isAgentOn;
}


///////////////////
void		DRTSimDocument::sporeShow ( bool state )
{
	Guard guard ( this->mutex() );
	_isSporeOn = state;

	// put animation toggle implementation here
	if ( _spore.shown () )
		_spore.hide();
	else
		_spore.show();

}

bool		DRTSimDocument::sporeShow ( )
{
	Guard guard ( this->mutex() );
	return _isSporeOn;
}


//////////////////
void		DRTSimDocument::stockShow ( bool state )
{
	Guard guard ( this->mutex() );
	_isStockOn = state;

	// put animation toggle implementation here
	if ( _stock.shown () )
		_stock.hide();
	else
		_stock.show();

}

bool		DRTSimDocument::stockShow ( )
{
	Guard guard ( this->mutex() );
	return _isStockOn;
}


///////////////////
void		DRTSimDocument::transShow ( bool state )
{
	Guard guard ( this->mutex() );
	_isTransOn = state;

	// put animation toggle implementation here
	if ( _trans.shown () )
		_trans.hide();
	else
		_trans.show();

}

bool		DRTSimDocument::transShow ( )
{
	Guard guard ( this->mutex() );
	return _isTransOn;
}

// end here ( Added by W.C )
///////////////////////////////////////////////////////////////////////////////
