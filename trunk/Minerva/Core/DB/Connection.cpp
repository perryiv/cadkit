
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Core/DB/Connection.h"

#include "Usul/Functions/Guid.h"
#include "Usul/System/Host.h"

#include "boost/algorithm/string/find.hpp"

#include "Serialize/XML/RegisterCreator.h"

#include "pqxx/pqxx"

#include <iostream>

using namespace Minerva::Core::DB;


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize static data members.
//
///////////////////////////////////////////////////////////////////////////////

//Connection::Pool Connection::_pool;


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
_connection ( static_cast < ConnectionType* > ( 0x0 ) ),
_connectionMutex ( ),
SERIALIZE_XML_INITIALIZER_LIST
{
  SERIALIZE_XML_ADD_MEMBER ( _host );
  SERIALIZE_XML_ADD_MEMBER ( _database );
  SERIALIZE_XML_ADD_MEMBER ( _user );
  SERIALIZE_XML_ADD_MEMBER ( _password );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::~Connection()
{
  this->disconnect();

  //delete _connectionMutex;
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
  try
  {
#if 0
    // Check to see if we already have a connection.
    Pool::iterator iter = _pool.find( this->name() );
    if( iter != _pool.end() )
      _connection = iter->second;
    else
    {
      // Set up a connection to the backend.
      _connection = ConnectionPtr ( this->_createConnection() );
      _pool.insert( Pool::value_type ( this->name(), _connection ) );
    }
#else
    // Set up a connection to the backend.
    _connection = ConnectionPtr ( this->_createConnection() );
#endif

  }
  catch ( const std::exception& e )
  {
    std::cerr << "Error 3839724500: " << e.what() << std::endl;
  }
  catch ( ... )
  {
    std::cerr << "Error 4295095950: Unknown exception caught while trying to connect." << std::endl;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create a connection.
//
///////////////////////////////////////////////////////////////////////////////

Connection::ConnectionType* Connection::_createConnection()
{
  std::ostringstream os;

#ifndef _MSC_VER
  std::string host ( Usul::System::Host::name() );
  if( boost::algorithm::find_first ( host, "viz" ) )
    _host = "cinema";
#endif

  os << "dbname=" << _database << " "
    << "user=" << _user << " "
    << "password=" << _password << " "
    << "host=" << _host;

  return new ConnectionType( os.str().c_str() );
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
    _connection->disconnect();
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Activate.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::activate()
{
  _connection->activate();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Deactivate.
//
///////////////////////////////////////////////////////////////////////////////

void Connection::deactivate()
{
  _connection->deactivate();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Scoped Connection Constructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::ScopedConnection::ScopedConnection ( Connection &c ) : _c ( c )
{
  _c.activate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Scoped Connection Destructor.
//
///////////////////////////////////////////////////////////////////////////////

Connection::ScopedConnection::~ScopedConnection ( ) 
{
  _c.deactivate();
}

///////////////////////////////////////////////////////////////////////////////
//
//  Execute a query.  Commit to ensure that it's executed now.
//
///////////////////////////////////////////////////////////////////////////////

pqxx::result Connection::executeQuery( const std::string& query ) const
{
  Guard guard ( _connectionMutex );
  pqxx::result result;

  if( 0x0 != _connection )
  {
    pqxx::work transaction ( *_connection, Usul::Functions::GUID::generate() );

    try
    {
      result = transaction.exec ( query );
      transaction.commit();
    }
    catch ( ... )
    {
      transaction.abort();
      std::cerr << "Query: " << query << " did not execute properly." << std::endl;
    }
  }

  return result;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute an insert query.
//
///////////////////////////////////////////////////////////////////////////////

int Connection::executeInsertQuery(const std::string &tableName, const Values &namesAndValues)
{
  if ( namesAndValues.size() == 0)
    throw std::runtime_error ("Error 1978842812: No names or values in the query");

  std::string names ( "( " );
  std::string values ( " VALUES ( " );

  for ( Values::const_iterator iter = namesAndValues.begin(); iter != namesAndValues.end(); ++iter )
  {
    names += iter->first;

    values += " '" + iter->second + "' ";

    if (iter != namesAndValues.end() - 1)
    {
      names += ",";
      values += ",";
    }
  }

  names += " )";
  values += " )";

  std::string query ( "INSERT INTO " + tableName + names + values );

  this->executeQuery(query);

  return this->_getMaxId( tableName );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the max id in the table.
//
///////////////////////////////////////////////////////////////////////////////

int Connection::_getMaxId( const std::string& table )
{
  std::string query ( "SELECT MAX(id) as id FROM " + table );

  pqxx::result result ( this->executeQuery( query ) );

  if( !result.empty() )
  {
    return result[0]["id"].as< int > ();
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

  pqxx::result result ( this->executeQuery ( query.str() ) );

  // If we have a row retured.
  if ( !result.empty() )
  {
    if( result[0][columnName].is_null() )
      return "";

    return result[0][columnName].as< std::string > ();
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

  pqxx::result result ( this->executeQuery ( query.str() ) );

  // If we have a row retured.
  if ( !result.empty() )
  {
    return result[0][columnName].as< double > ();
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

  pqxx::result r ( this->executeQuery ( query ) );

  min = r[0]["lowest"].as < double > ();
  max = r[0]["highest"].as < double > ();
}
