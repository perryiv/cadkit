
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_DB_CONNECTION_POOL_H__
#define __MINERVA_CORE_DB_CONNECTION_POOL_H__

#include "Minerva/DataSources/PG/Export.h"
#include "Minerva/DataSources/PG/Connection.h"

#include "Usul/Threads/Guard.h"
#include "Usul/Threads/Mutex.h"

#include <string>
#include <map>

namespace Minerva {
namespace DataSources {
namespace PG {

class MINERVA_POSTGRES_EXPORT ConnectionPool
{
public:
  typedef std::pair<std::string, std::string> Key;
	typedef std::map<Key, std::string> Passwords;
  
  static ConnectionPool& instance();

  bool                             hasConnection ( const std::string& name ) const;
  Connection*                      getConnection ( const std::string& name );
  void                             addConnection ( Connection* connection );

	bool                             hasPassword ( const std::string& user, const std::string& host ) const;
	std::string                      getPassword ( const std::string& user, const std::string& host ) const;
	void                             setPassword ( const std::string& user, const std::string& host, const std::string& password );

protected:
  ConnectionPool();
  ~ConnectionPool();

private:

	typedef Usul::Threads::Mutex Mutex;
	typedef Usul::Threads::Guard<Mutex> Guard;
  typedef std::map< std::string, Connection::RefPtr > Connections;

	mutable Mutex *_mutex;
  Connections _connections;
	Passwords _passwords;

  static ConnectionPool *_instance;
};

}
}
}

#endif // __MINERVA_CORE_DB_CONNECTION_POOL_H__
