
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DB_CONNECTION_H__
#define __DB_CONNECTION_H__

#include "Minerva/DataSources/PG/Export.h"
#include "Minerva/Interfaces/IDatabaseConnection.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"

#include "Serialize/XML/Macros.h"

#include "boost/shared_ptr.hpp"

// Forward declarations.
namespace pqxx
{
  class result;
  template < typename T > class basic_connection;
  class connect_direct;
  class connect_lazy;
  typedef basic_connection < connect_direct > connection;
}

#include <string>

namespace Minerva {
namespace DataSources {
namespace PG {

class MINERVA_POSTGRES_EXPORT Connection : public Usul::Base::Referenced,
                                           public Minerva::Interfaces::IDatabaseConnection
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::Mutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;
  typedef std::pair< std::string, std::string > StringPair;
  typedef std::vector < StringPair >  Values;
  typedef pqxx::basic_connection < pqxx::connect_lazy > ConnectionType;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( Connection );
  
  /// IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Connection();

  /// Build connection string for postgres.
  std::string          connectionString();

  /// Get the name of the connection
  std::string          name() const;

  /// Get/Set hostname.
  void                 hostname( const std::string& host );
  const std::string&   hostname() const;

  /// Get/Set database.
  void                 database( const std::string& db );
  const std::string&   database() const;

  /// Get/Set username.
  void                 username( const std::string& user );
  const std::string&   username() const;

  /// Get/Set password.
  void                 password( const std::string& pw );
  const std::string&   password() const;

  /// Connect.
  void                 connect();

  /// Disconnect.
  void                 disconnect();

  /// Activate.
  void                 activate();

  /// Deactivate
  void                 deactivate();

  /// Execute the query.
  pqxx::result         executeQuery( const std::string& ) const;

  /// Execute the query with a timeout value.
  pqxx::result         executeQuery( const std::string& query, unsigned int seconds ) const;

  /// Execute insert query.
  int                  executeInsertQuery( const std::string& tableName, const Values& values );

  std::string          getColumnDataString ( const std::string& tableName, int id, const std::string& columnName );
  double               getColumnDataDouble ( const std::string& tableName, int id, const std::string& columnName );

  // Get max and min values of the given field name.
  void                 getMinAndMax ( const std::string& tableName, const std::string& fieldName, double& min, double& max );

  struct MINERVA_POSTGRES_EXPORT ScopedConnection
  {
    ScopedConnection ( Connection &c );
    ~ScopedConnection();

  private:
    Connection &_c;
  };

	virtual void deserialize ( const XmlTree::Node &node );

protected:
  virtual ~Connection();

  /// Get the maximium primary id in the given table.
  int                  _getMaxId( const std::string& table );

private:

  typedef boost::shared_ptr < ConnectionType >  ConnectionPtr;

  std::string _host;
  std::string _database;
  std::string _user;
  std::string _password;

  ConnectionPtr _connection;

  mutable Mutex *_connectionMutex;

  SERIALIZE_XML_DEFINE_MAP;
	SERIALIZE_XML_CLASS_NAME ( Connection );
	SERIALIZE_XML_SERIALIZE_FUNCTION;
	SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};

}
}
}


#endif // __DB_CONNECTION_H__
