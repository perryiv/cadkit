
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Document/CommandReceiver.h"

#include "Minerva/DataSources/Result.h"

#include "Minerva/Core/Serialize.h"

#include "Usul/Interfaces/ICommand.h"
#include "Usul/Interfaces/ICommandQueueAdd.h"

#include "Usul/Trace/Trace.h"

#include <sstream>
#include <iostream>

using namespace Minerva::Document;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandReceiver::CommandReceiver() :
  BaseClass (),
  _connection ( 0x0 ),
  _sessionID( 0 ),
  _lastCommandID ( 0 ),
  _timeout ( 60 ),
  _connected ( false )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CommandReceiver::~CommandReceiver()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the application connection.
//
///////////////////////////////////////////////////////////////////////////////

void CommandReceiver::connection( Minerva::DataSources::Connection *connection )
{
  _connection = connection;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Are we connected to the session?
//
///////////////////////////////////////////////////////////////////////////////

bool CommandReceiver::connected() const
{
  return _connected;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect to a session.
//
///////////////////////////////////////////////////////////////////////////////

void CommandReceiver::connectToSession( const std::string& name )
{
  std::ostringstream query;
  query << "SELECT * FROM wnv_sessions WHERE name = '" << name << "'";

  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query.str() ) );

  if ( result->prepareNextRow() )
  {
    _sessionID = result->asUInt ( "id" );
    _connected = true;
  }
  else
  {
    // Clear the query.
    query.str("");

    query << "INSERT INTO wnv_sessions ( name ) VALUES ( '" << name << "' )";

    _connection->executeQuery( query.str() );

    // Try and connect again.
    this->connectToSession ( name );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Update the scene.
//
///////////////////////////////////////////////////////////////////////////////

void CommandReceiver::processCommands ( Usul::Interfaces::IUnknown *caller )
{
  // Safely...
  try
  {
    this->_processCommands ( caller );
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
//  Process Commands.
//
///////////////////////////////////////////////////////////////////////////////

void CommandReceiver::_processCommands ( Usul::Interfaces::IUnknown *caller )
{
  USUL_TRACE_SCOPE;

  // Create the query.
  std::ostringstream query;
  query << "SELECT * FROM commands WHERE id > " << _lastCommandID << " AND session_id = " << _sessionID;

  // Get the result.
  Minerva::DataSources::Result::RefPtr result ( _connection->executeQuery ( query.str() ) );
 
  if ( result->numRows() > 0 )
    std::cout << "Processing " << result->numRows() << " commands." << std::endl;

  // While we have more work to do...
  unsigned int current ( 0 );
  while ( result->prepareNextRow() )
  {
    std::cout << "Processing command " << ++current << " of " << result->numRows() << " commands." << std::endl;

    // Get the id of the row we are processing.
    unsigned int rowId ( result->asUInt ( "id" ) );

    // Get the xml data.
    std::string xml ( result->asString ( "xml_data" ) );

    // Deserialize.
    Usul::Interfaces::ICommand::QueryPtr command ( Minerva::Core::deserializeCommand ( xml ) );

    Usul::Interfaces::ICommandQueueAdd::QueryPtr queue ( caller );

    // Add the command to the queue...
    if( command.valid() && queue.valid() )
      queue->addCommand ( command.get() );

    // Remember the last id we processed.
    _lastCommandID = rowId;
  }
}
