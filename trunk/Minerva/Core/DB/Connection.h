
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

#include "Minerva/Core/Export.h"

#include "Usul/Threads/Mutex.h"
#include "Usul/Threads/Guard.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "Serialize/XML/Macros.h"

#include "boost/shared_ptr.hpp"

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
namespace Core {
namespace DB {

class MINERVA_EXPORT Connection : public Usul::Base::Referenced
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
  USUL_DECLARE_REF_POINTERS ( Connection );

  Connection();

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

  /// Execute insert query.
  int                  executeInsertQuery( const std::string& tableName, const Values& values );

  std::string          getColumnDataString ( const std::string& tableName, int id, const std::string& columnName );
  double               getColumnDataDouble ( const std::string& tableName, int id, const std::string& columnName );

  // Get max and min values of the given field name.
  void                 getMinAndMax ( const std::string& tableName, const std::string& fieldName, double& min, double& max );

  struct ScopedConnection
  {
    ScopedConnection ( Connection &c );
    ~ScopedConnection();

  private:
    Connection &_c;
  };

protected:
  virtual ~Connection();

  /// Get the maximium primary id in the given table.
  int                  _getMaxId( const std::string& table );

  /// Create Connection.
  ConnectionType*          _createConnection();

private:

  typedef boost::shared_ptr < ConnectionType >  ConnectionPtr;

  std::string _host;
  std::string _database;
  std::string _user;
  std::string _password;

  ConnectionPtr _connection;

  mutable Mutex *_connectionMutex;

  SERIALIZE_XML_DEFINE_MAP;
public:
  virtual const char *className() const { return "Connection"; }
  virtual void serialize ( XmlTree::Node &parent ) const
  {
    _dataMemberMap.serialize ( parent );
  }
  virtual void deserialize ( const XmlTree::Node &node )
  {
    _dataMemberMap.deserialize ( node );
    this->connect();
  }
protected:
  template < class T > void _addMember ( const std::string &name, T &value )
  {
    _dataMemberMap.addMember ( name, value );
  }
};

}
}
}


#endif // __DB_CONNECTION_H__
