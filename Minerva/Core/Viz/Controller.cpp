
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/Viz/Controller.h"
#include "Minerva/Core/Viz/Progress.h"
#include "Minerva/Core/postGIS/Geometry.h"

#include "Usul/Types/Types.h"
#include "Usul/Endian/Endian.h"
#include "Usul/File/Temp.h"

#include "osg/ref_ptr"

#include <sstream>
#include <iostream>
#include <fstream>

using namespace Minerva::Core::Viz;


USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( Controller , Controller::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Controller::Controller( ) :
_sceneManager( new Minerva::Core::Scene::SceneManager ),
_callback( 0x0 ),
_update ( 1000 ),
_applicationConnection ( 0x0 ),
_sessionID( 0 ),
_lastEventID ( 0 )
{
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
//  Are there any events to process?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::hasEvents()
{
  std::ostringstream query;
  query << "SELECT MAX(id) FROM wnv_event_table WHERE session_id = " << _sessionID;
  pqxx::result r ( _applicationConnection->executeQuery ( query.str() ) );

  if ( !r.empty() && !r[0][0].is_null() )
  {
    return ( r[0][0].as< int >() > _lastEventID );
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are there any events to process?
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_getNextEvent( int& type, std::string& tableName, int &eventId )
{
  std::ostringstream query;
  query << "SELECT * FROM wnv_event_table WHERE id > " << _lastEventID << " AND session_id = " << _sessionID << " LIMIT 1";

  pqxx::result result ( _applicationConnection->executeQuery ( query.str() ) );

  if( result.empty() )
    throw std::runtime_error ("Error 4176339188: No new draw command data in the database." );

  // Get the row.
  pqxx::result::tuple row ( result[0] );

  // Get the id.
  int id ( row["id"].as< int > ( ) );

  // Get the command type
  type = row["type"].as< int >( );

  // Get the event id.
  eventId = row["event_id"].as<int>( );

  std::ostringstream os;
  os << "SELECT * FROM wnv_event_types WHERE id = " << type;

  pqxx::result r ( _applicationConnection->executeQuery ( os.str() ) );

  if( r.empty() )
    throw std::runtime_error("Error 545121892: No data in wnv_event_types" );

  // table for the criteria.
  tableName = r[0]["table_name"].as< std::string > ();

  _lastEventID = id;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Should there be animation?
//
///////////////////////////////////////////////////////////////////////////////

bool Controller::_getAnimateCommand( const std::string animateTable, int eventID, bool &accumulate, bool &dateTimeStep, float &speed )
{
  std::ostringstream query;
  query << "SELECT * FROM " << animateTable << " WHERE id = " << eventID << " AND session_id = " << _sessionID;;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str() ) );

  if( !result.empty() && !result[0]["animate"].is_null() )
  {
    speed = result[0]["speed"].as < float > ();
    accumulate = result[0]["accumulate"].as < bool > ();
    dateTimeStep = result[0]["date_time_step"].as < bool > ();
    return result[0]["animate"].as< bool > ();
  }

  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//   Update the progress if we should.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_updateProgress()
{
  // If we should update...
  if( _update() )
  {
    // Render for progress.
    if( _callback.valid() )
      _callback->draw();
  }
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
  // While we have more work to do...
  while ( this->hasEvents() )
  {
    // Variables needed to figure out what to draw.
    int type ( 0 ), eventID ( 0 );
    std::string tableName ( "" );

    // Get the data.
    this->_getNextEvent ( type, tableName, eventID );

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
    };
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process Add Layer.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processAddLayer( const std::string& drawCommandTable, int eventID )
{
  // Get the data.
  Minerva::Core::Layers::Layer::RefPtr layer ( this->_getLayer ( drawCommandTable, eventID ) );

  if ( false == layer.valid () )
  {
    std::cerr << "Could not create layer." << std::endl;
    return;
  }

  // Check to see if we already have this layer.
  bool hasLayer ( _sceneManager->hasLayer( layer->guid() ) );

  Minerva::Core::Viz::Progress::RefPtr progress ( new Minerva::Core::Viz::Progress );

  if ( hasLayer )
  {
    layer->modify( progress.get() );
  }
  else 
  {
    _sceneManager->addLayer( layer );
    layer->buildDataObjects( progress.get() );
  }
  
  _sceneManager->dirty( true );
  _sceneManager->buildScene();

  // Render for progress.
  this->_updateProgress();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the layer data.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::Core::Layers::Layer* Controller::_getLayer( const std::string drawCommandTable, int eventID )
{
  std::ostringstream query;
  query << "SELECT * FROM " << drawCommandTable << " WHERE id = " << eventID << " AND session_id = " << _sessionID;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str() ) );

  if( result.empty() )
    throw std::runtime_error ("Error 1607371066: No new layer data in the database." );

  // Get the row.
  pqxx::result::tuple row ( result[0] );

  // Get the xml data.
  std::string xml ( row["xml_data"].as< std::string > () );

  Minerva::Core::Layers::Layer::RefPtr layer ( Minerva::Core::deserialize( xml ) );

  if( layer.valid() )
  {
    // Check to see if we already have this layer.
    bool hasLayer ( _sceneManager->hasLayer( layer->guid() ) );

    if ( hasLayer )
    {
      Minerva::Core::Layers::Layer::RefPtr oldLayer ( _sceneManager->getLayer( layer->guid() ) );
      Minerva::Core::deserialize( xml, oldLayer.get() );
      return oldLayer.get();
    }
  }

  return layer.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process Remove Layer.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processRemoveLayer( const std::string& drawCommandTable, int eventID )
{
  Minerva::Core::Layers::Layer::RefPtr layer ( this->_getLayer ( drawCommandTable, eventID ) );

  if( layer.valid() )
  {
    // Remove the layer.
    _sceneManager->removeLayer( layer->guid() );
    _sceneManager->dirty ( true );
    _sceneManager->buildScene();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Process animation.
//
///////////////////////////////////////////////////////////////////////////////

void Controller::_processAnimation( const std::string& tableName, int eventID )
{
  std::ostringstream query;
  query << "SELECT * FROM " << tableName << " WHERE id = " << eventID << " AND session_id = " << _sessionID;;

  pqxx::result result ( _applicationConnection->executeQuery ( query.str() ) );

  if( !result.empty() && !result[0]["animate"].is_null() )
  {
    float speed           ( result[0]["speed"].as < float > () );
    bool accumulate       ( result[0]["accumulate"].as < bool > () );
    bool timeWindow       ( result[0]["time_window"].as< bool > () );
    bool animate          ( result[0]["animate"].as< bool > () );
    unsigned int numDays  ( result[0]["num_days_to_show"].as < unsigned int > () );

    _sceneManager->animate ( animate, accumulate, speed, timeWindow, numDays );
    _sceneManager->buildScene();
  }
}

