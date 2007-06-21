
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Viz/Controller.h"
#include "Minerva/Core/Viz/AddLayerJob.h"
#include "Minerva/Core/postGIS/Geometry.h"
#include "Minerva/Core/RegisterFactories.h"
#include "Minerva/Core/Serialize.h"

#include "Usul/Interfaces/IPlayMovie.h"

#include "Usul/Types/Types.h"
#include "Usul/Endian/Endian.h"
#include "Usul/File/Temp.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Trace/Trace.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Console/Feedback.h"

#include "Usul/Components/Manager.h"
#include "osg/ref_ptr"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace Minerva::Core::Viz;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Controller , Controller::BaseClass );

// TODO: This should be moved into it's own file.
class MovieLayer : public Minerva::Core::Layers::Layer
{
	public:

	typedef Minerva::Core::Layers::Layer	BaseClass;

	USUL_DECLARE_REF_POINTERS ( MovieLayer );

	MovieLayer() : BaseClass(), 
		_position( 0.0, 0.0, 0.0 ),
		_width( 0.0, 0.0, 0.0 ),
		_height( 0.0, 0.0, 0.0 ), 
		_fileName( std::string( "" ) )		
	{					
	}

	virtual Usul::Interfaces::IUnknown* clone() const 
	{
    MovieLayer::RefPtr layer ( new MovieLayer );                  // Reference count is now 1
    Usul::Interfaces::IUnknown::QueryPtr unknown ( layer.get() ); // Reference count is now 2
    layer.release();                                              // Reference count is now 1
    return unknown.release();                                     // Reference count is now 0
	}	  

	void buildDataObjects( Usul::Interfaces::IUnknown* ){}

	void modify( Usul::Interfaces::IUnknown* ){} 

	void buildScene( osg::Group* gr )
	{
    if( gr )
    {
      // Call play movie function and pass in group, width, height, position, and path.
      Usul::Interfaces::IPlayMovie::QueryPtr playMoviePlug ( Usul::Components::Manager::instance().getInterface( Usul::Interfaces::IPlayMovie::IID ) );

      if( playMoviePlug.valid() )
      {
        gr->addChild( playMoviePlug->playMovie( _position, _width, _height, _fileName ) );
      }
    }	
	}

	void setValues( const osg::Vec3f& position, const osg::Vec3f& width, const osg::Vec3f& height, const std::string& fileName )
	{
		_position = position;
		_width    = width;
		_height   = height;
		_fileName = fileName;
	}  

	protected:
	
	osg::Vec3f  _position;
	osg::Vec3f  _width;
	osg::Vec3f  _height;

	std::string _fileName;	 
};

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Controller( Usul::Interfaces::IUnknown *caller ) :
_sceneManager( 0x0 ),
_applicationConnection ( 0x0 ),
_sessionID( 0 ),
_lastEventID ( 0 ),
_lastCommandID ( 0 ),
_timeout ( 60 ),
_caller ( caller )
{
  // Make sure we have factories registered.
  Minerva::Core::registerFactories();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::~Controller()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *Controller::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the application connection.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::applicationConnection( Minerva::Core::DB::Connection *connection )
{
  _applicationConnection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a session.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::connectToSession( const std::string& name )
{
  std::ostringstream query;
  query << "SELECT * FROM wnv_sessions WHERE name = '" << name << "'";

  pqxx::result result ( _applicationConnection->executeQuery ( query.str() ) );

  if ( !result.empty() )
  {
    _sessionID = result[0]["id"].as < unsigned int > ();
  }
  else
  {
    // Clear the query.
    query.str("");

    query << "INSERT INTO wnv_sessions ( name ) VALUES ( '" << name << "' )";

    _applicationConnection->executeQuery( query.str() );

    // Try and connect again.
    this->connectToSession ( name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the tablename for the given event.
//
///////////////////////////////////////////////////////////////////////////////

std::string Controller::_getTablename( int type )
{
  USUL_TRACE_SCOPE;

  std::ostringstream os;
  os << "SELECT * FROM wnv_event_types WHERE id = " << type;

  pqxx::result r ( _applicationConnection->executeQuery ( os.str(), _timeout ) );

  if( r.empty() )
    throw std::runtime_error("Error 545121892: No data in wnv_event_types" );

  // table for the criteria.
  return r[0]["table_name"].as< std::string > ();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::updateScene( )
{
  // Safely...
  try
  {
    this->_processEvents();
    this->_processCommands();
  }

  // TODO: if an expection is thrown, then the offending row should be removed so it doesn't get tried again.
  catch ( const std::exception& e )
  {
    std::cerr << "Standared exception caught while processing event: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "Unknown exception caught while processing events. " << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process draw commands.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processEvents()
{
  USUL_TRACE_SCOPE;

  // Create the query.
  std::ostringstream query;
  query << "SELECT * FROM wnv_event_table WHERE id > " << _lastEventID << " AND session_id = " << _sessionID;

  // Get the result.
  pqxx::result result ( _applicationConnection->executeQuery ( query.str(), _timeout ) );

  if( result.size() > 0 )
    std::cerr << "Processing " << result.size() << " events..." << std::endl;

  // While we have more work to do...
  for ( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    // Get the id of the row we are processing.
    unsigned int rowId ( iter["id"].as < unsigned int > () );

    // Variables needed to figure out what to draw.
    int type ( iter["type"].as < int >() );
    unsigned int eventID ( iter["event_id"].as < unsigned int > () );

    // Get the table name to pull command from.
    std::string tableName ( this->_getTablename ( type ) );

    // Redirect to the proper function.
    switch ( type )
    {
    case 1:
      this->_processAddLayer ( tableName, eventID );
      break;
    case 2:
      this->_processAnimation ( tableName, eventID );
      break;
    case 3:
      this->_processRemoveLayer ( tableName, eventID );
      break;
    case 4:
      this->_processPlayMovie ( tableName, eventID );
    };

    // Remember the last id we processed.
    _lastEventID = rowId;

  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process Commands.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processCommands()
{
  USUL_TRACE_SCOPE;

  // Create the query.
  std::ostringstream query;
  query << "SELECT * FROM commands WHERE id > " << _lastCommandID << " AND session_id = " << _sessionID;

  // Get the result.
  pqxx::result result ( _applicationConnection->executeQuery ( query.str(), _timeout ) );

  // While we have more work to do...
  for ( pqxx::result::const_iterator iter = result.begin(); iter != result.end(); ++iter )
  {
    // Get the id of the row we are processing.
    unsigned int rowId ( iter["id"].as < unsigned int > () );

    // Get the xml data.
    std::string xml ( iter["xml_data"].as< std::string > () );

    // Deserialize.
    Usul::Interfaces::ICommand::QueryPtr command ( Minerva::Core::deserializeCommand ( xml ) );

    if( command.valid() )
      command->execute ( _caller.get() );

    // Remember the last id we processed.
    _lastCommandID = rowId;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process Add Layer.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processAddLayer( const std::string& drawCommandTable, unsigned int eventID )
{
  USUL_TRACE_SCOPE;

  // Get the data.
  Minerva::Core::Layers::Layer::RefPtr layer ( this->_getLayer ( drawCommandTable, eventID ) );

  if ( false == layer.valid () )
  {
    std::cerr << "Could not create layer." << std::endl;
    return;
  }

  // Progress feedback.
  Usul::Interfaces::IUnknown::QueryPtr progress ( new Usul::Console::Feedback );

  // Always remove.  This isn't optimal, but it's more stable.
  _sceneManager->removeLayer ( layer->guid() );

  // Create a job to add layer to scene.
  Usul::Jobs::Manager::instance().add ( new AddLayerJob ( _sceneManager.get(), layer, progress.get() ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer data.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* Controller::_getLayer( const std::string drawCommandTable, unsigned int eventID )
{
  USUL_TRACE_SCOPE;

  std::ostringstream query;
  query << "SELECT * FROM " << drawCommandTable << " WHERE id = " << eventID << " AND session_id = " << _sessionID;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str(), _timeout ) );

  if( result.empty() )
    throw std::runtime_error ("Error 1607371066: No new layer data in the database." );

  // Get the row.
  pqxx::result::tuple row ( result[0] );

  // Get the xml data.
  std::string xml ( row["xml_data"].as< std::string > () );

  // Deserialize.
  Minerva::Core::Layers::Layer::RefPtr layer ( Minerva::Core::deserialize( xml ) );

  // Return the layer.
  return layer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process Remove Layer.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processRemoveLayer( const std::string& drawCommandTable, unsigned int eventID )
{
  Minerva::Core::Layers::Layer::RefPtr layer ( this->_getLayer ( drawCommandTable, eventID ) );

  if( layer.valid() )
  {
    // Remove the layer.
    _sceneManager->removeLayer( layer->guid() );

    // Dirty the scene.
    _sceneManager->dirty ( true );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processAnimation( const std::string& tableName, unsigned int eventID )
{
  std::ostringstream query;
  query << "SELECT * FROM " << tableName << " WHERE id = " << eventID << " AND session_id = " << _sessionID;;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str(), _timeout ) );

  if( !result.empty() && !result[0]["animate"].is_null() )
  {
    // Get the animation flag.
    bool animate          ( result[0]["animate"].as< bool > () );

    if( animate )
    {
      float speed           ( result[0]["speed"].as < float > () );
      bool accumulate       ( result[0]["accumulate"].as < bool > () );
      bool timeWindow       ( result[0]["time_window"].as< bool > () );
      unsigned int numDays  ( result[0]["num_days_to_show"].as < unsigned int > () );
      OsgTools::Animate::Settings::TimestepType type ( static_cast < OsgTools::Animate::Settings::TimestepType > ( result[0]["timestep_type"].as < unsigned int > () ) );
      _sceneManager->timestepType( type );
      _sceneManager->showPastEvents ( accumulate );
      _sceneManager->animationSpeed ( speed );
      _sceneManager->timeWindow ( timeWindow );
      _sceneManager->timeWindowDuration ( numDays );
      _sceneManager->startAnimation();
    }
    else
      _sceneManager->stopAnimation();

    // Dirty the scene.
    _sceneManager->dirty ( true );
  }
}


std::istream& operator >> ( std::istream& in, osg::Vec3f& v )
{
  in >> v.x() >> v.y() >> v.z();
  return in;
}

///////////////////////////////////////////////////////////////////////////////
//
//  Process animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processPlayMovie   ( const std::string& tableName, unsigned int eventID )
{
  std::ostringstream query;
  query << "SELECT * FROM " << tableName << " WHERE id = " << eventID << " AND session_id = " << _sessionID;;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str(), _timeout ) );

  if( !result.empty() )
  {
    std::string postionString ( result[0]["the_position"].as< std::string >() );
    std::string widthString   ( result[0]["width"].as< std::string >() );
    std::string heightString  ( result[0]["height"].as< std::string >() );    
    std::string path          ( result[0]["path"].as < std::string > () );

    osg::Vec3f position, width, height;
    std::istringstream in ( postionString );
    in >> position;

    in.clear();     
    in.str( widthString ); 
    in >> width;

    in.clear();
    in.str( heightString );
    in >> height;


    MovieLayer::RefPtr movie =  new MovieLayer();
    movie->setValues( position, width, height, path );

    if( movie.valid() )
    {
      _sceneManager->addLayer( movie.get() );

      // Dirty the scene.
      _sceneManager->dirty ( true );
    }
  }
}
