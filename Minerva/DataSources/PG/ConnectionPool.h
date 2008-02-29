
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

#include <string>
#include <map>

namespace Minerva {
namespace DataSources {
namespace PG {

class MINERVA_POSTGRES_EXPORT ConnectionPool
{
public:
  static ConnectionPool& instance();

  bool                             hasConnection ( const std::string& name ) const;
  Connection*                      getConnection ( const std::string& name );
  void                             addConnection ( Connection* connection );

protected:
  ConnectionPool();
  ~ConnectionPool();

private:
  typedef std::map< std::string, Connection::RefPtr > Connections;

  Connections _connections;

  static ConnectionPool *_instance;
};

}
}
}

#endif // __MINERVA_CORE_DB_CONNECTION_POOL_H__
