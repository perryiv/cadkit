
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/DataSources/PG/Connection.h"
#include "Minerva/DataSources/PG/ConnectionPool.h"
#include "Minerva/DataSources/PG/Result.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Components/Manager.h"
#include "Usul/Factory/RegisterCreator.h"
#include "Usul/Functions/GUID.h"
#include "Usul/Interfaces/IPasswordPrompt.h"
#include "Usul/Scope/Caller.h"
#include "Usul/Strings/Case.h"
#include "Usul/Strings/Format.h"
#include "Usul/Trace/Trace.h"

#include "libpq-fe.h"

#include <iostream>

using namespace Minerva::DataSources::PG;


///////////////////////////////////////////////////////////////////////////////
//
//  Throw an exception.
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  void throwException ( const std::string& id, const std::string& query, const std::string& error )
  {
    std::ostringstream message;
    message << "Error " << id <<": Error occured while executing: " << query << std::endl <<
      "Reason: " << error;
    throw std::runtime_error ( message.str() );
  }
}


USUL_FACTORY_REGISTER_CREATOR ( Connection );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS( Connection, Connection::BaseClass );

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::Connection() : BaseClass(),
	_host (),
	_database (),
	_user (),
	_password(),
	_connection ( 0x0 ),
	_connectionMutex ( Mutex::create () ),
	SERIALIZE_XML_INITIALIZER_LIST
{
  SERIALIZE_XML_ADD_MEMBER ( _host );
  SERIALIZE_XML_ADD_MEMBER ( _database );
  SERIALIZE_XML_ADD_MEMBER ( _user );
	// Don't serialize or deserialize password in clear text.
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::~Connection()
{
  this->disconnect();

  delete _connectionMutex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the name.
//
///////////////////////////////////////////////////////////////////////////////

std::string Connection::name() const
{
  return this->database() + " on " + this->hostname();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the hostname.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::hostname( const std::string& host )
{
  _host = host;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the hostname.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Connection::hostname() const
{
  return _host;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the database.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::database( const std::string& db )
{
  _database = db;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the database.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Connection::database() const
{
  return _database;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the username
//
///////////////////////////////////////////////////////////////////////////////

void Connection::username( const std::string& user )
{
  _user = user;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the username.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Connection::username() const
{
  return _user;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the password.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::password( const std::string& pw )
{
  _password = pw;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the password.
//
///////////////////////////////////////////////////////////////////////////////

const std::string& Connection::password() const
{
  return _password;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Connect.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::connect()
{
	const unsigned int maxTries ( 3 );
	unsigned int tries ( 0 );

  // Make sure we aren't connected.
  this->disconnect();

	do
	{
		// Increate the number of tries.
		++tries;

		try
		{
      // Set up a connection to the backend.
      _connection = ::PQconnectdb ( this->connectionString().c_str() );

      if ( CONNECTION_OK != ::PQstatus ( _connection ) )
      {
        std::cout << "Error 3671765373: Connection to database failed: " << ::PQerrorMessage ( _connection ) << std::endl;
        this->disconnect();
      }
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error 3839724500: " << e.what() << std::endl;
		}
		catch ( ... )
		{
			std::cerr << "Error 4295095950: Unknown exception caught while trying to connect." << std::endl;
		}

	} while ( 0x0 == _connection || tries < maxTries );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Build a connection string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Connection::connectionString()
{
	// Check to see if the password has been cached.
	if ( true == ConnectionPool::instance().hasPassword( this->username(), this->hostname() ) )
		this->password ( ConnectionPool::instance().getPassword ( this->username(), this->hostname() ) );

	// Check to see if we have a password.
	if ( true == this->password().empty() )
	{
		typedef Usul::Components::Manager PluginManager;
		typedef Usul::Interfaces::IPasswordPrompt IPasswordPrompt;

		IPasswordPrompt::QueryPtr pp ( PluginManager::instance().getInterface ( IPasswordPrompt::IID ) );

		if ( pp.valid() )
		{
			const std::string prompt ( Usul::Strings::format ( "Enter password for ", this->username(), " on ", this->hostname() ) );
			this->password ( pp->promptForPassword ( prompt ) );
			ConnectionPool::instance().setPassword ( this->username(), this->hostname(), this->password() );
		}
	}

  // Connection parameters.
  std::string hostname ( Usul::Strings::format (     "host=", this->hostname() ) );
  std::string database ( Usul::Strings::format (   "dbname=", this->database() ) );
  std::string username ( Usul::Strings::format (     "user=", this->username() ) );
  std::string password ( Usul::Strings::format ( "password=", this->password() ) );
    
  return Usul::Strings::format ( hostname, " ", username, " ", password, " ", database );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Disconnect.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::disconnect()
{
  if( _connection )
  {
    ::PQfinish ( _connection );
    _connection = 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scoped Connection Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::ScopedConnection::ScopedConnection ( Connection &c ) : _c ( c )
{
  _c.connect ();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Scoped Connection Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::ScopedConnection::~ScopedConnection ( ) 
{
  _c.disconnect();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Is the string a select statement?
//
///////////////////////////////////////////////////////////////////////////////

namespace Helper
{
  inline bool isSelectStatement ( const std::string &sql )
  {
    const std::string s ( Usul::Strings::lowerCase ( sql ) );
    if ( s.size() >= 6 )
    {
      return ( ( 's' == s[0] ) && 
               ( 'e' == s[1] ) && 
               ( 'l' == s[2] ) && 
               ( 'e' == s[3] ) && 
               ( 'c' == s[4] ) && 
               ( 't' == s[5] ) );
    }
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute a query.
//
///////////////////////////////////////////////////////////////////////////////

Minerva::DataSources::Result* Connection::executeQuery ( const std::string& query ) const
{
  USUL_TRACE_SCOPE;

  // Only one thread at a time can execute a query.
  Guard guard ( *_connectionMutex );

  Minerva::DataSources::PG::Result::RefPtr result ( 0x0 );

  if ( 0x0 != _connection )
  {
    PGresult *pqResult ( ::PQexec ( _connection, query.c_str() ) );
    
    ExecStatusType status ( ::PQresultStatus ( pqResult ) );

    // Check for a fatal error.
    if ( PGRES_FATAL_ERROR == status )
    {
      Helper::throwException ( "4111576506", query, ::PQerrorMessage ( _connection ) );
    }
    
    // Check for error depending on the query type.
    if ( Helper::isSelectStatement ( query ) )
    {
      if ( PGRES_TUPLES_OK != status )
        Helper::throwException ( "3115192614", query, ::PQerrorMessage ( _connection ) );
    }
    else if ( PGRES_COMMAND_OK != status )
    {
      Helper::throwException ( "1038329512", query, ::PQerrorMessage ( _connection ) );
    }
    
    result = new Minerva::DataSources::PG::Result ( pqResult );
  }

  return result.release();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a transaction.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::startTransaction() const
{
  USUL_TRACE_SCOPE;
  if ( 0x0 == _connection )
    return;

  PGresult *result ( ::PQexec ( _connection, "BEGIN" ) );
  Usul::Scope::Caller::RefPtr clear ( Usul::Scope::makeCaller ( Usul::Adaptors::bind1 ( result, ::PQclear ) ) );

  if ( PGRES_COMMAND_OK != ::PQresultStatus ( result ) )
  {
    throw std::runtime_error ( "Error : BEGIN command failed: " + std::string ( ::PQerrorMessage ( _connection ) ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Start a transaction.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::endTransaction() const
{
  USUL_TRACE_SCOPE;
  if ( 0x0 == _connection )
    return;

  PGresult *result ( ::PQexec ( _connection, "END" ) );
  ::PQclear ( result );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max id in the table.
//
///////////////////////////////////////////////////////////////////////////////

int Connection::_getMaxId( const std::string& table )
{
  std::string query ( "SELECT MAX(id) as id FROM " + table );

  Minerva::DataSources::Result::RefPtr result ( this->executeQuery ( query ) );

  if ( result->prepareNextRow() )
  {
    return result->asInt ( "id" );
  }

  throw std::runtime_error("Error 1172716358: Could not get id from the data base.");
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the column data as a string.
//
///////////////////////////////////////////////////////////////////////////////

std::string Connection::getColumnDataString ( const std::string& tableName, int id, const std::string& columnName )
{
  std::ostringstream query;
  query << "SELECT " << columnName << " FROM " << tableName << " WHERE id = " << id;

  Minerva::DataSources::Result::RefPtr result ( this->executeQuery ( query.str() ) );

  // If we have a row retured.
  if ( result->prepareNextRow() )
  {
    if( result->isNull ( columnName ) )
      return "";

    return result->asString ( columnName );
  }

  return "";
}


///////////////////////////////////////////////////////////////////////////////
//
//   Get the column data as a double.
//
///////////////////////////////////////////////////////////////////////////////

double Connection::getColumnDataDouble ( const std::string& tableName, int id, const std::string& columnName )
{
	std::ostringstream query;
  query << "SELECT " << columnName << " FROM " << tableName << " WHERE id = " << id;

  Minerva::DataSources::Result::RefPtr result ( this->executeQuery ( query.str() ) );

  // If we have a row retured.
  if ( result->prepareNextRow() )
  {
    return result->asDouble ( columnName );
  }

  return 0.0;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get min and max.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::getMinAndMax ( const std::string& tableName, const std::string& fieldName, double& min, double& max )
{
  std::string query ( "SELECT MAX(" + fieldName + ") as \"highest\", MIN(" + fieldName + ") as \"lowest\" FROM " + tableName );

  Minerva::DataSources::Result::RefPtr r ( this->executeQuery ( query ) );

  if ( r->prepareNextRow() )
  {
    min = r->asDouble ( "lowest" );
    max = r->asDouble ( "highest" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for an interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown* Connection::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
    case Usul::Interfaces::IUnknown::IID:
    case Minerva::Interfaces::IDatabaseConnection::IID:
      return static_cast < Minerva::Interfaces::IDatabaseConnection* > ( this );
    default:
      return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deserialize.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::deserialize ( const XmlTree::Node &node )
{
  _dataMemberMap.deserialize ( node );
  this->connect();
}
