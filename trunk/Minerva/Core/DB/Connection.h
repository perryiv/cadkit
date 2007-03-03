
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

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include "pqxx/pqxx"

#include "boost/serialization/nvp.hpp"
#include "boost/serialization/split_member.hpp"

#include <string>

namespace boost { namespace serialization { class access; } }

namespace Minerva {
namespace Core {
namespace DB {

class MINERVA_EXPORT Connection : public Usul::Base::Referenced
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef std::pair< std::string, std::string > StringPair;
  typedef std::vector < StringPair >  Values;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Connection );

  Connection();

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

  /// Execute the query.
  pqxx::result         executeQuery( const std::string& ) const;

  /// Execute insert query.
  int                  executeInsertQuery( const std::string& tableName, const Values& values );

  std::string          getColumnDataString ( const std::string& tableName, int id, const std::string& columnName );
	double               getColumnDataDouble ( const std::string& tableName, int id, const std::string& columnName );

  // Get max and min values of the given field name.
  void                 getMinAndMax ( const std::string& tableName, const std::string& fieldName, double& min, double& max );

protected:
  virtual ~Connection();

  /// Get the maximium primary id in the given table.
  int                  _getMaxId( const std::string& table );

private:
  friend class boost::serialization::access;
  template<class Archive> void save(Archive & ar, const unsigned int version) const;
  template<class Archive> void load(Archive & ar, const unsigned int version);
  BOOST_SERIALIZATION_SPLIT_MEMBER()

  std::string _host;
  std::string _database;
  std::string _user;
  std::string _password;

  pqxx::connection* _connection;
};

template< class Archive >
void Connection::save( Archive &ar, const unsigned int version ) const
{
  ar & boost::serialization::make_nvp ( "Host", _host );
  ar & boost::serialization::make_nvp ( "Database", _database );
  ar & boost::serialization::make_nvp ( "User", _user );
  ar & boost::serialization::make_nvp ( "Password", _password );
}

template< class Archive >
void Connection::load( Archive &ar, const unsigned int version )
{
  ar & boost::serialization::make_nvp ( "Host", _host );
  ar & boost::serialization::make_nvp ( "Database", _database );
  ar & boost::serialization::make_nvp ( "User", _user );
  ar & boost::serialization::make_nvp ( "Password", _password );

  this->connect();
}

}
}
}

#endif // __DB_CONNECTION_H__
